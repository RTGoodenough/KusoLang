

#include "generator/generator.hpp"

#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>

#include <belt/overload.hpp>
#include <variant>

#include "generator/context.hpp"
#include "lexer/token.hpp"
#include "linux/linux.hpp"
#include "parser/ast.hpp"
#include "x64/addressing.hpp"
#include "x64/x64.hpp"

// TODO(rolland): add semantic errors function

namespace kuso {
/**
 * @brief Generates x64 assembly from an AST
 * 
 * @param ast AST to generate from
 */
void Generator::generate(const AST& ast) {
  emit("global    _start\nsection   .text\n_start:\n");
  for (const auto& statement : ast) {
    generate(statement);
  }

  emit(x64::Op::MOV, x64::Register::RDI, x64::Literal{0});
  emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{lnx::Syscall::EXIT});
  emit(x64::Op::SYSCALL);
  _outputFile.write(_output_data);
  _outputFile.write(_output_code);
}

/**
 * @brief Generates x64 assembly from a statement
 * 
 * @param statement Statement to generate from
 */
void Generator::generate(const AST::Statement& statement) {
  belt::overloaded_visit(
      statement.statement,
      [&](const std::unique_ptr<AST::Declaration>& declaration) { generate_declaration(*declaration); },
      [&](const std::unique_ptr<AST::Assignment>& assignment) { generate_assignment(*assignment); },
      [&](const std::unique_ptr<AST::Exit>& exit) { generate_exit(*exit); },
      [&](const std::unique_ptr<AST::Print>& print) { generate_print(*print); },
      [&](const std::unique_ptr<AST::If>& ifStatement) { generate_if(*ifStatement); },
      [&](const std::unique_ptr<AST::Type>& type) { generate_type(*type); },
      [&](const std::unique_ptr<AST::While>& whileStatement) { generate_while(*whileStatement); },
      [&](const std::unique_ptr<AST::Push>&) {}, [&](const std::unique_ptr<AST::Return>&) {},
      [](std::nullptr_t) {});
}

/**
 * @brief Generates x64 assembly from a declaration
 * 
 * @param declaration Declaration to generate from
 */
void Generator::generate_declaration(const AST::Declaration& declaration) {
  auto&       current = context();
  const auto& name = get_identifier(declaration);
  auto        typeID = get_type_id(get_decl_type(declaration));

  if (current.variables.find(name) != current.variables.end()) {
    throw std::runtime_error("Multiple Declarations of " + name);
  }

  auto& type = get_type(typeID);

  if (declaration.value) {
    if (type.offsets) throw std::runtime_error("Cannot assign value to type with attributes");
    generate_expression(*declaration.value);
  } else {
    if (type.offsets) {
      std::for_each(type.offsets.value().begin(), type.offsets.value().end(),
                    [&](auto&) { push(x64::Literal{0}); });
    } else {
      push(x64::Literal{0});
    }
  }

  current.variables[name] =
      Variable{typeID, {x64::Address::Mode::INDIRECT_DISPLACEMENT, x64::Register::RSP, 0}};
}

/**
 * @brief Generates x64 assembly from an assignment
 * 
 * @param assignment Assignment to generate from
 */
void Generator::generate_assignment(const AST::Assignment& assignment) {
  // TODO(rolland): check if assignment is valid
  const auto& name = get_identifier(assignment);

  auto variableIter = context().variables.find(name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + name);
  }

  generate_expression(*assignment.value);
  pop(x64::Register::RAX);
  emit(x64::Op::MOV, get_location(*assignment.dest), x64::Register::RAX);
}

/**
 * @brief Generates x64 assembly from an expression
 * 
 * @param expression Expression to generate from
 */
void Generator::generate_expression(const AST::Expression& expression) {
  generate_expression(*expression.value);
}

/**
 * @brief Generates x64 assembly from an equality expression
 * 
 * @param equality Equality to generate from
 */
void Generator::generate_expression(const AST::Equality& equality) {
  if (equality.right) {
    generate_expression(*equality.right);
  }
  generate_expression(*equality.left);

  if (equality.right) {
    pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    emit(x64::Op::CMP, x64::Register::RAX, x64::Register::RDX);
    pull_comparison_result(equality.equal ? AST::BinaryOp::EQ : AST::BinaryOp::NEQ);
    push(x64::Register::RAX);
  }
}

/**
 * @brief Generates x64 assembly from a comparison expression
 * 
 * @param comparison Comparison to generate from
 */
void Generator::generate_expression(const AST::Comparison& comparison) {
  if (comparison.right) {
    generate_expression(*comparison.right);
  }
  generate_expression(*comparison.left);

  if (comparison.right) {
    pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    emit(x64::Op::CMP, x64::Register::RAX, x64::Register::RDX);
    pull_comparison_result(comparison.op);
    push(x64::Register::RAX);
  }
}

/**
 * @brief Generates x64 assembly from a term expression
 * 
 * @param term Term to generate from
 */
void Generator::generate_expression(const AST::Term& term) {
  if (term.right) {
    generate_expression(*term.right);
  }
  generate_expression(*term.left);

  if (term.right) {
    pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    if (term.op == AST::BinaryOp::ADD)
      emit(x64::Op::ADD, x64::Register::RAX, x64::Register::RDX);
    else if (term.op == AST::BinaryOp::SUB)
      emit(x64::Op::SUB, x64::Register::RAX, x64::Register::RDX);
    push(x64::Register::RAX);
  }
}

/**
 * @brief Generates x64 assembly from a factor expression
 * 
 * @param factor Factor to generate from
 */
void Generator::generate_expression(const AST::Factor& factor) {
  if (factor.right) {
    generate_expression(*factor.right);
  }
  generate_expression(*factor.left);

  if (factor.right) {
    pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    if (factor.op == AST::BinaryOp::MUL)
      emit(x64::Op::IMUL, x64::Register::RAX, x64::Register::RDX);
    else if (factor.op == AST::BinaryOp::DIV)
      emit(x64::Op::DIV, x64::Register::RAX, x64::Register::RDX);
    push(x64::Register::RAX);
  }
}

/**
 * @brief Generates x64 assembly from a unary expression
 * 
 * @param unary Unary to generate from
 */
void Generator::generate_expression(const AST::Unary& unary) {
  belt::overloaded_visit(
      unary.value, [&](const std::unique_ptr<AST::Primary>& primary) { generate_expression(*primary); },
      [&](const std::unique_ptr<AST::Unary>& unary) { generate_expression(*unary); }, [](std::nullptr_t) {});
  if (unary.op == AST::BinaryOp::SUB || unary.op == AST::BinaryOp::NOT) {
    pop(x64::Register::RAX);
    emit(x64::Op::NEG, x64::Register::RAX);
    push(x64::Register::RAX);
  }
}

/**
 * @brief Generates x64 assembly from a primary expression
 * 
 * @param primary Primary to generate from
 */
void Generator::generate_expression(const AST::Primary& primary) {
  belt::overloaded_visit(
      primary.value, [&](const std::unique_ptr<AST::Terminal>& terminal) { generate_expression(*terminal); },
      [&](const std::unique_ptr<AST::Expression>& expression) { generate_expression(*expression); },
      [&](const std::unique_ptr<AST::String>& string) { generate_string(*string); },
      [&](const std::unique_ptr<AST::Variable>& variable) { generate_expression(*variable); });
}

/**
 * @brief Generates x64 assembly from a terminal expression
 * 
 * @param terminal Terminal to generate from
 */
void Generator::generate_expression(const AST::Terminal& terminal) {
  belt::overloaded_visit(
      terminal.value, [&](const std::unique_ptr<AST::Variable>& variable) { generate_expression(*variable); },
      [&](const Token& token) { generate_expression(token); },
      [&](const std::unique_ptr<AST::String>& string) { generate_string(*string); }, [](std::nullptr_t) {});
}

/**
 * @brief Generates x64 assembly from a variable expression
 * 
 * @param variable Variable to generate from
 */
void Generator::generate_expression(const AST::Variable& variable) {
  auto variableIter = context().variables.find(variable.name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + variable.name);
  }
  push(get_location(variable));
}

/**
 * @brief Generates x64 assembly from a token
 * 
 * @param token Token to generate from
 */
void Generator::generate_expression(const Token& token) {
  if (token.type == Token::Type::NUMBER) {
    push(x64::Literal{std::stoi(token.value)});
  } else {
    throw std::runtime_error("Invalid Terminal");
  }
}

/**
 * @brief Generates x64 assembly from an exit statement
 * 
 * @param exit Exit to generate from
 */
void Generator::generate_exit(const AST::Exit& exit) {
  if (exit.value) {
    generate_expression(*exit.value);
    pop(x64::Register::RAX);
  } else {
    emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{0});
  }
  emit(x64::Op::MOV, x64::Register::RDI, x64::Register::RAX);
  emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{lnx::Syscall::EXIT});
  emit(x64::Op::SYSCALL);
}

/**
 * @brief Generates x64 assembly from a return statement
 * 
 * @param returnStatement Return to generate from
 */
void Generator::generate_print(const AST::Print& print) {
  generate_expression(*print.value);
  pop(x64::Register::RAX);
  emit(x64::Op::MOV, x64::Register::RSI, x64::Register::RAX);
  emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{lnx::Syscall::WRITE});
  emit(x64::Op::MOV, x64::Register::RDI, x64::Literal{1});
  emit(x64::Op::SYSCALL);
}

/**
 * @brief Adds a new type to the current context
 * 
 * @param type Type to create
 */
void Generator::generate_type(const AST::Type& type) {
  auto& current = context();

  auto typeIter = current.typeIDs.find(type.name);
  if (typeIter != current.typeIDs.end()) {
    throw std::runtime_error("Multiple Declarations of " + type.name);
  }

  auto typeID = current.currVariable;
  ++current.currVariable;

  Type_t newType;

  if (!type.attributes.empty()) {
    newType.offsets = std::map<std::string, int>{};
    int currOffset = 0;

    for (const auto& attribute : type.attributes) {
      auto& refType = get_type(get_type_id(attribute.type));
      newType.offsets.value()[attribute.name] = currOffset;
      currOffset += refType.size;
      newType.size += refType.size;
    }
  } else {
    newType.size = x64::Size::QWORD;
  }

  current.types[typeID] = newType;
  current.typeIDs[type.name] = typeID;
}

/**
 * @brief Generates x64 assembly from a string
 * 
 * @param string String to generate from
 */
void Generator::generate_string(const AST::String& string) {
  auto replacedStr = std::regex_replace(string.value, std::regex{"\\\\n"}, "\", 10, \"");
  if (replacedStr.length() >= 2 && replacedStr.substr(replacedStr.length() - 2) == "\"\"") {
    replacedStr = replacedStr.substr(0, replacedStr.length() - 2);
  }

  auto iter = _string_names.find(replacedStr);
  if (iter == _string_names.end()) {
    auto name = fmt::format("str_{}", _string_names.size());
    _string_names[replacedStr] = name;
    _string_values[name] = replacedStr;
    add_data(name, replacedStr);
  }

  // TODO(rolland): should we be using rdx?
  emit(x64::Op::MOV, x64::Register::RAX, _string_names[replacedStr]);
  emit(x64::Op::MOV, x64::Register::RDX, x64::Literal{static_cast<int>(replacedStr.length())});
}

/**
 * @brief Generates x64 assembly from an if statement
 * 
 * @param ifNode If to generate from
 */
auto Generator::generate_if(const AST::If& ifNode) -> void {
  generate_expression(*ifNode.condition);
  pop(x64::Register::RAX);
  emit(x64::Op::CMP, x64::Register::RAX, x64::Literal{0});

  // TODO(rolland): this doesn't work for more than one if statement in the same context
  auto elseLabel = fmt::format("else_{}", _contexts.size());
  auto endLabel = fmt::format("end_{}", _contexts.size());

  if (!ifNode.elseBody.empty()) {
    emit(x64::Op::JE, elseLabel);
  } else {
    emit(x64::Op::JE, endLabel);
  }

  // enter_context();
  for (const auto& statement : ifNode.body) {
    generate(statement);
  }
  // leave_context();

  if (!ifNode.elseBody.empty()) {
    emit(x64::Op::JMP, endLabel);
    emit(fmt::format("{}:", elseLabel));
    // enter_context();
    for (const auto& statement : ifNode.elseBody) {
      generate(statement);
    }
    // leave_context();
  }

  emit(fmt::format("{}:", endLabel));
}

/**
 * @brief Generates x64 assembly from a while statement
 * 
 * @param whileStatement While to generate from
 */
void Generator::generate_while(const AST::While& whileStatement) {
  auto startLabel = fmt::format("start_{}", _contexts.size());
  auto endLabel = fmt::format("end_{}", _contexts.size());

  emit(fmt::format("{}:", startLabel));
  generate_expression(*whileStatement.condition);
  pop(x64::Register::RAX);
  emit(x64::Op::CMP, x64::Register::RAX, x64::Literal{0});
  emit(x64::Op::JE, endLabel);

  // enter_context();
  for (const auto& statement : whileStatement.body) {
    generate(statement);
  }
  // leave_context();

  emit(x64::Op::JMP, startLabel);
  emit(fmt::format("{}:", endLabel));
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% HELPERS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Creates a new context
 * 
 */
void Generator::enter_context() {
  auto& current = context();
  _contexts.emplace(Context{.size = 0,
                            .stack = current.stack,
                            .variables = current.variables,
                            .currVariable = current.currVariable,
                            .typeIDs = current.typeIDs,
                            .types = current.types});
}

/**
 * @brief Removes the current context
 * 
 */
void Generator::leave_context() {
  auto& current = context();
  std::for_each(current.variables.begin(), current.variables.end(), [&](auto&) { pop(x64::Register::RAX); });
  current.stack.disp += current.size;
  _contexts.pop();
}

/**
 * @brief Generates a push instruction from an address
 * 
 * @param addr Address to push from
 */
void Generator::push(x64::Address addr) {
  auto& current = context();
  emit(x64::Op::PUSH, x64::Size::QWORD, addr);
  current.stack.disp -= x64::Size::QWORD;
  current.size += x64::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x64::Size::QWORD;
  }
}

/**
 * @brief Generates a push instruction from a literal
 * 
 * @param lit Literal to push
 */
void Generator::push(x64::Literal lit) {
  auto& current = context();
  emit(x64::Op::PUSH, x64::Size::QWORD, lit);
  current.stack.disp -= x64::Size::QWORD;
  current.size += x64::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x64::Size::QWORD;
  }
}

/**
 * @brief Generates a push instruction from a register
 * 
 * @param reg Register to push from
 */
void Generator::push(x64::Register reg) {
  auto& current = context();
  emit(x64::Op::PUSH, x64::Size::QWORD, reg);
  current.stack.disp -= x64::Size::QWORD;
  current.size += x64::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x64::Size::QWORD;
  }
}

/**
 * @brief Generates a pop instruction from an address
 * 
 * @param addr Address to pop to
 */
void Generator::pop(x64::Register reg) {
  auto& current = context();
  emit(x64::Op::POP, x64::Size::QWORD, reg);
  current.stack.disp += x64::Size::QWORD;
  current.size -= x64::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp -= x64::Size::QWORD;
  }
}

/**
 * @brief Appends a string to the output code
 * 
 * @param value String to append
 */
void Generator::emit(const std::string& value) { _output_code.append(fmt::format("{}\n", value)); }

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 */
void Generator::emit(x64::Op operation) {
  _output_code.append(fmt::format("{}\n", x64::to_string(operation)));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 */
void Generator::emit(x64::Op operation, x64::Register src) {
  _output_code.append(fmt::format("{} {}\n", x64::to_string(operation), x64::to_string(src)));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, const std::string& value) {
  _output_code.append(fmt::format("{} {}\n", x64::to_string(operation), value));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Address dest, x64::Address src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), dest.to_string(), src.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Address addr) {
  _output_code.append(fmt::format("{} {}\n", x64::to_string(operation), addr.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Register dest, x64::Address src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), x64::to_string(dest), src.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Address dest, x64::Register src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), dest.to_string(), x64::to_string(src)));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Address dest, x64::Literal value) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), dest.to_string(), value.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Register dest, x64::Literal value) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), x64::to_string(dest), value.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Register dest, x64::Register src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), x64::to_string(dest), x64::to_string(src)));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param dest Destination of the operation
 * @param src Source of the operation
 */
void Generator::emit(x64::Op operation, x64::Register dest, const std::string& value) {
  _output_code.append(fmt::format("{} {}, {}\n", x64::to_string(operation), x64::to_string(dest), value));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param value Value of the operation
 */
void Generator::emit(x64::Op operation, x64::Literal value) {
  _output_code.append(fmt::format("{} {}\n", x64::to_string(operation), value.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param value Value of the operation
 */
void Generator::emit(x64::Op operation, x64::Size size, x64::Register reg) {
  _output_code.append(
      fmt::format("{} {} {}\n", x64::to_string(operation), x64::to_string(size), x64::to_string(reg)));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param value Value of the operation
 */
void Generator::emit(x64::Op operation, x64::Size size, x64::Address addr) {
  _output_code.append(
      fmt::format("{} {} {}\n", x64::to_string(operation), x64::to_string(size), addr.to_string()));
}

/**
 * @brief Generates an x64 instruction
 * 
 * @param operation Operation to generate
 * @param value Value of the operation
 */
void Generator::emit(x64::Op operation, x64::Size size, x64::Literal lit) {
  _output_code.append(
      fmt::format("{} {} {}\n", x64::to_string(operation), x64::to_string(size), lit.to_string()));
}

/**
 * @brief Returns the correct comparison operation for a given binary operation
 * 
 * @param operation Operation to generate from
 */
void Generator::pull_comparison_result(AST::BinaryOp operation) {
  switch (operation) {
    case AST::BinaryOp::EQ:
      emit(x64::Op::SETE, x64::Register::AL);
      break;
    case AST::BinaryOp::NEQ:
      emit(x64::Op::SETNE, x64::Register::AL);
      break;
    case AST::BinaryOp::LT:
      emit(x64::Op::SETL, x64::Register::AL);
      break;
    case AST::BinaryOp::LTE:
      emit(x64::Op::SETLE, x64::Register::AL);
      break;
    case AST::BinaryOp::GT:
      emit(x64::Op::SETG, x64::Register::AL);
      break;
    case AST::BinaryOp::GTE:
      emit(x64::Op::SETGE, x64::Register::AL);
      break;
    default:
      throw std::runtime_error("Invalid Comparison Operation");
  }
  emit(x64::Op::MOVZX, x64::Register::RAX, x64::Register::AL);
}

/**
 * @brief Adds a new data entry to the output data
 * 
 * @param name name of the data
 * @param value value of the data
 */
void Generator::add_data(const std::string& name, const std::string& value) {
  _output_data.append(fmt::format("{}: db {}\n", name, value));
  _output_data.append(fmt::format("{}_len: equ {}\n", name, value.length()));
}

Generator::Generator(const std::filesystem::path& outputpath)
    : _outputFile(outputpath, std::ios_base::out | std::ios_base::trunc) {
  if (!_outputFile.is_open()) {
    throw std::runtime_error("Failed to open output file");
  }
  init_context();
  init_data();
}

/**
 * @brief Returns the type with the given typeID
 * 
 * @param typeID ID of the type to get
 * @return Type_t& Type with the given typeID
 */
auto Generator::get_type(int typeID) -> Type_t& {
  auto typeIter = context().types.find(typeID);
  if (typeIter == context().types.end()) {
    throw std::runtime_error("Unknown Type " + std::to_string(typeID));
  }

  return typeIter->second;
}

/**
 * @brief Returns the typeID of the given type
 * 
 * @param type Type to get the ID of
 * @return int ID of the given type
 */
auto Generator::get_type_id(const std::string& type) -> int {
  auto typeIter = context().typeIDs.find(type);
  if (typeIter == context().typeIDs.end()) {
    throw std::runtime_error("Unknown Type " + type);
  }

  return typeIter->second;
}

/**
 * @brief Returns the destination of the given assignment
 * 
 * @param assignment Assignment to get the destination of
 * @return const std::string& Destination of the given assignment
 */
[[nodiscard]] auto Generator::get_identifier(const AST::Assignment& assignment) -> const std::string& {
  return assignment.dest->name;
}

/**
 * @brief Returns the location of the given variable
 * 
 * @param variable Variable to get the location of
 * @return x64::Address Location of the given variable
 */
auto Generator::get_location(const AST::Variable& variable) -> x64::Address {
  auto variableIter = context().variables.find(variable.name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + variable.name);
  }

  int offset = 0;
  if (variable.attribute) {
    auto& type = get_type(variableIter->second.type);
    offset = type.get_offset(variable.attribute.value());
  }

  return variableIter->second.location + offset;
}

/**
 * @brief Returns the identifier of the given terminal
 * 
 * @param terminal Terminal to get the identifier of
 * @return const std::string& Identifier of the given terminal
 */
auto Generator::get_identifier(const AST::Terminal& terminal) -> const std::string& {
  return belt::overloaded_visit<const std::string&>(
      terminal.value, [&](const std::unique_ptr<AST::Variable>& variable) { return variable->name; },
      [&](const Token& token) { return token.value; },
      [&](const std::unique_ptr<AST::String>& string) { return string->value; });
}

/**
 * @brief Returns the identifier of the given declaration
 * 
 * @param declaration Declaration to get the identifier of
 * @return const std::string& Identifier of the given declaration
 */
auto Generator::get_identifier(const AST::Declaration& declaration) -> const std::string& {
  return declaration.name;
}

/**
 * @brief Returns the type of the given declaration
 * 
 * @param declaration Declaration to get the type of
 * @return const std::string& Type of the given declaration
 */
auto Generator::get_decl_type(const AST::Declaration& declaration) -> const std::string& {
  return declaration.type;
}

/**
 * @brief Initializes the starting context
 * 
 */
void Generator::init_context() {
  _contexts.emplace(Context{
      .size = 0,
      .stack = x64::Address{x64::Address::Mode::DIRECT, x64::Register::RSP, 0},
      .variables = {},
      .currVariable = 2,
      .typeIDs = {{"int", 0}, {"str", 1}},
      .types = {{0, Type_t{x64::Size::QWORD, std::nullopt}}, {1, Type_t{x64::Size::QWORD, std::nullopt}}}});
}

/**
 * @brief Initializes the starting data
 * 
 */
void Generator::init_data() { _output_data = "section   .data\n"; }
}  // namespace kuso