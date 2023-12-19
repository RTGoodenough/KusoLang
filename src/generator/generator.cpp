

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
#include "x86/addressing.hpp"
#include "x86/x86.hpp"

// TODO(rolland): add semantic errors function

namespace kuso {
void Generator::generate(const AST& ast) {
  emit("global    _start\nsection   .text\n_start:\n");
  for (const auto& statement : ast) {
    generate(statement);
  }

  emit(x86::Op::MOV, x86::Register::RDI, x86::Literal{0});
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::EXIT});
  emit(x86::Op::SYSCALL);
  _outputFile.write(_output_data);
  _outputFile.write(_output_code);
}

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
                    [&](auto&) { push(x86::Literal{0}); });
    } else {
      push(x86::Literal{0});
    }
  }

  current.variables[name] =
      Variable{typeID, {x86::Address::Mode::INDIRECT_DISPLACEMENT, x86::Register::RSP, 0}};
}

void Generator::generate_assignment(const AST::Assignment& assignment) {
  // TODO(rolland): check if assignment is valid
  const auto& name = get_identifier(assignment);

  auto variableIter = context().variables.find(name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + name);
  }

  generate_expression(*assignment.value);
  pop(x86::Register::RAX);
  emit(x86::Op::MOV, get_location(*assignment.dest), x86::Register::RAX);
}

void Generator::generate_expression(const AST::Expression& expression) {
  generate_expression(*expression.value);
}

void Generator::generate_expression(const AST::Equality& equality) {
  if (equality.right) {
    generate_expression(*equality.right);
  }
  generate_expression(*equality.left);

  if (equality.right) {
    pop(x86::Register::RAX);
    pop(x86::Register::RDX);
    emit(x86::Op::CMP, x86::Register::RAX, x86::Register::RDX);
    pull_comparison_result(equality.equal ? AST::BinaryOp::EQ : AST::BinaryOp::NEQ);
    push(x86::Register::RAX);
  }
}

void Generator::generate_expression(const AST::Comparison& comparison) {
  if (comparison.right) {
    generate_expression(*comparison.right);
  }
  generate_expression(*comparison.left);

  if (comparison.right) {
    pop(x86::Register::RAX);
    pop(x86::Register::RDX);
    emit(x86::Op::CMP, x86::Register::RAX, x86::Register::RDX);
    pull_comparison_result(comparison.op);
    push(x86::Register::RAX);
  }
}

void Generator::generate_expression(const AST::Term& term) {
  if (term.right) {
    generate_expression(*term.right);
  }
  generate_expression(*term.left);

  if (term.right) {
    pop(x86::Register::RAX);
    pop(x86::Register::RDX);
    if (term.op == AST::BinaryOp::ADD)
      emit(x86::Op::ADD, x86::Register::RAX, x86::Register::RDX);
    else if (term.op == AST::BinaryOp::SUB)
      emit(x86::Op::SUB, x86::Register::RAX, x86::Register::RDX);
    push(x86::Register::RAX);
  }
}

void Generator::generate_expression(const AST::Factor& factor) {
  if (factor.right) {
    generate_expression(*factor.right);
  }
  generate_expression(*factor.left);

  if (factor.right) {
    pop(x86::Register::RAX);
    pop(x86::Register::RDX);
    if (factor.op == AST::BinaryOp::MUL)
      emit(x86::Op::IMUL, x86::Register::RAX, x86::Register::RDX);
    else if (factor.op == AST::BinaryOp::DIV)
      emit(x86::Op::DIV, x86::Register::RAX, x86::Register::RDX);
    push(x86::Register::RAX);
  }
}

void Generator::generate_expression(const AST::Unary& unary) {
  belt::overloaded_visit(
      unary.value, [&](const std::unique_ptr<AST::Primary>& primary) { generate_expression(*primary); },
      [&](const std::unique_ptr<AST::Unary>& unary) { generate_expression(*unary); }, [](std::nullptr_t) {});
  if (unary.op == AST::BinaryOp::SUB || unary.op == AST::BinaryOp::NOT) {
    pop(x86::Register::RAX);
    emit(x86::Op::NEG, x86::Register::RAX);
    push(x86::Register::RAX);
  }
}

void Generator::generate_expression(const AST::Primary& primary) {
  belt::overloaded_visit(
      primary.value, [&](const std::unique_ptr<AST::Terminal>& terminal) { generate_expression(*terminal); },
      [&](const std::unique_ptr<AST::Expression>& expression) { generate_expression(*expression); },
      [&](const std::unique_ptr<AST::String>& string) { generate_string(*string); },
      [&](const std::unique_ptr<AST::Variable>& variable) { generate_expression(*variable); });
}

void Generator::generate_expression(const AST::Terminal& terminal) {
  belt::overloaded_visit(
      terminal.value, [&](const std::unique_ptr<AST::Variable>& variable) { generate_expression(*variable); },
      [&](const Token& token) { generate_expression(token); },
      [&](const std::unique_ptr<AST::String>& string) { generate_string(*string); }, [](std::nullptr_t) {});
}

void Generator::generate_expression(const AST::Variable& variable) {
  auto variableIter = context().variables.find(variable.name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + variable.name);
  }
  push(get_location(variable));
}

void Generator::generate_expression(const Token& token) {
  if (token.type == Token::Type::NUMBER) {
    push(x86::Literal{std::stoi(token.value)});
  } else {
    throw std::runtime_error("Invalid Terminal");
  }
}

void Generator::generate_exit(const AST::Exit& exit) {
  if (exit.value) {
    generate_expression(*exit.value);
    pop(x86::Register::RAX);
  } else {
    emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{0});
  }
  emit(x86::Op::MOV, x86::Register::RDI, x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::EXIT});
  emit(x86::Op::SYSCALL);
}

void Generator::generate_print(const AST::Print& print) {
  generate_expression(*print.value);
  pop(x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RSI, x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::WRITE});
  emit(x86::Op::MOV, x86::Register::RDI, x86::Literal{1});
  emit(x86::Op::SYSCALL);
}

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
    newType.size = x86::Size::QWORD;
  }

  current.types[typeID] = newType;
  current.typeIDs[type.name] = typeID;
}

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
  emit(x86::Op::MOV, x86::Register::RAX, _string_names[replacedStr]);
  emit(x86::Op::MOV, x86::Register::RDX, x86::Literal{static_cast<int>(replacedStr.length())});
}

auto Generator::generate_if(const AST::If& ifNode) -> void {
  generate_expression(*ifNode.condition);
  pop(x86::Register::RAX);
  emit(x86::Op::CMP, x86::Register::RAX, x86::Literal{0});

  // TODO(rolland): this doesn't work for more than one if statement in the same context
  auto elseLabel = fmt::format("else_{}", _contexts.size());
  auto endLabel = fmt::format("end_{}", _contexts.size());

  if (!ifNode.elseBody.empty()) {
    emit(x86::Op::JE, elseLabel);
  } else {
    emit(x86::Op::JE, endLabel);
  }

  // enter_context();
  for (const auto& statement : ifNode.body) {
    generate(statement);
  }
  // leave_context();

  if (!ifNode.elseBody.empty()) {
    emit(x86::Op::JMP, endLabel);
    emit(fmt::format("{}:", elseLabel));
    // enter_context();
    for (const auto& statement : ifNode.elseBody) {
      generate(statement);
    }
    // leave_context();
  }

  emit(fmt::format("{}:", endLabel));
}

void Generator::generate_while(const AST::While& whileStatement) {
  auto startLabel = fmt::format("start_{}", _contexts.size());
  auto endLabel = fmt::format("end_{}", _contexts.size());

  emit(fmt::format("{}:", startLabel));
  generate_expression(*whileStatement.condition);
  pop(x86::Register::RAX);
  emit(x86::Op::CMP, x86::Register::RAX, x86::Literal{0});
  emit(x86::Op::JE, endLabel);

  // enter_context();
  for (const auto& statement : whileStatement.body) {
    generate(statement);
  }
  // leave_context();

  emit(x86::Op::JMP, startLabel);
  emit(fmt::format("{}:", endLabel));
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% HELPERS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Generator::enter_context() {
  auto& current = context();
  _contexts.emplace(Context{.size = 0,
                            .stack = current.stack,
                            .variables = current.variables,
                            .currVariable = current.currVariable,
                            .typeIDs = current.typeIDs,
                            .types = current.types});
}

void Generator::leave_context() {
  auto& current = context();
  std::for_each(current.variables.begin(), current.variables.end(), [&](auto&) { pop(x86::Register::RAX); });
  current.stack.disp += current.size;
  _contexts.pop();
}

void Generator::push(x86::Address addr) {
  auto& current = context();
  emit(x86::Op::PUSH, x86::Size::QWORD, addr);
  current.stack.disp -= x86::Size::QWORD;
  current.size += x86::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x86::Size::QWORD;
  }
}

void Generator::push(x86::Literal lit) {
  auto& current = context();
  emit(x86::Op::PUSH, x86::Size::QWORD, lit);
  current.stack.disp -= x86::Size::QWORD;
  current.size += x86::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x86::Size::QWORD;
  }
}

void Generator::push(x86::Register reg) {
  auto& current = context();
  emit(x86::Op::PUSH, x86::Size::QWORD, reg);
  current.stack.disp -= x86::Size::QWORD;
  current.size += x86::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp += x86::Size::QWORD;
  }
}

void Generator::pop(x86::Register reg) {
  auto& current = context();
  emit(x86::Op::POP, x86::Size::QWORD, reg);
  current.stack.disp += x86::Size::QWORD;
  current.size -= x86::Size::QWORD;
  for (auto& variable : current.variables) {
    variable.second.location.disp -= x86::Size::QWORD;
  }
}

void Generator::emit(const std::string& value) { _output_code.append(fmt::format("{}\n", value)); }
void Generator::emit(x86::Op operation) {
  _output_code.append(fmt::format("{}\n", x86::to_string(operation)));
}
void Generator::emit(x86::Op operation, x86::Register src) {
  _output_code.append(fmt::format("{} {}\n", x86::to_string(operation), x86::to_string(src)));
}
void Generator::emit(x86::Op operation, const std::string& value) {
  _output_code.append(fmt::format("{} {}\n", x86::to_string(operation), value));
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Address src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), dest.to_string(), src.to_string()));
}
void Generator::emit(x86::Op operation, x86::Address addr) {
  _output_code.append(fmt::format("{} {}\n", x86::to_string(operation), addr.to_string()));
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Address src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), x86::to_string(dest), src.to_string()));
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Register src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), dest.to_string(), x86::to_string(src)));
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Literal value) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), dest.to_string(), value.to_string()));
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Literal value) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), x86::to_string(dest), value.to_string()));
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Register src) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x86::to_string(operation), x86::to_string(dest), x86::to_string(src)));
}
void Generator::emit(x86::Op operation, x86::Register dest, const std::string& value) {
  _output_code.append(fmt::format("{} {}, {}\n", x86::to_string(operation), x86::to_string(dest), value));
}
void Generator::emit(x86::Op operation, x86::Literal value) {
  _output_code.append(fmt::format("{} {}\n", x86::to_string(operation), value.to_string()));
}

void Generator::emit(x86::Op operation, x86::Size size, x86::Register reg) {
  _output_code.append(
      fmt::format("{} {} {}\n", x86::to_string(operation), x86::to_string(size), x86::to_string(reg)));
}

void Generator::emit(x86::Op operation, x86::Size size, x86::Address addr) {
  _output_code.append(
      fmt::format("{} {} {}\n", x86::to_string(operation), x86::to_string(size), addr.to_string()));
}

void Generator::emit(x86::Op operation, x86::Size size, x86::Literal lit) {
  _output_code.append(
      fmt::format("{} {} {}\n", x86::to_string(operation), x86::to_string(size), lit.to_string()));
}

void Generator::pull_comparison_result(AST::BinaryOp operation) {
  switch (operation) {
    case AST::BinaryOp::EQ:
      emit(x86::Op::SETE, x86::Register::AL);
      break;
    case AST::BinaryOp::NEQ:
      emit(x86::Op::SETNE, x86::Register::AL);
      break;
    case AST::BinaryOp::LT:
      emit(x86::Op::SETL, x86::Register::AL);
      break;
    case AST::BinaryOp::LTE:
      emit(x86::Op::SETLE, x86::Register::AL);
      break;
    case AST::BinaryOp::GT:
      emit(x86::Op::SETG, x86::Register::AL);
      break;
    case AST::BinaryOp::GTE:
      emit(x86::Op::SETGE, x86::Register::AL);
      break;
    default:
      throw std::runtime_error("Invalid Comparison Operation");
  }
  emit(x86::Op::MOVZX, x86::Register::RAX, x86::Register::AL);
}

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

auto Generator::get_type(int typeID) -> Type_t& {
  auto typeIter = context().types.find(typeID);
  if (typeIter == context().types.end()) {
    throw std::runtime_error("Unknown Type " + std::to_string(typeID));
  }

  return typeIter->second;
}

auto Generator::get_type_id(const std::string& type) -> int {
  auto typeIter = context().typeIDs.find(type);
  if (typeIter == context().typeIDs.end()) {
    throw std::runtime_error("Unknown Type " + type);
  }

  return typeIter->second;
}

[[nodiscard]] auto Generator::get_identifier(const AST::Assignment& assignment) -> const std::string& {
  return assignment.dest->name;
}

auto Generator::get_location(const AST::Variable& variable) -> x86::Address {
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

auto Generator::get_identifier(const AST::Terminal& terminal) -> const std::string& {
  return belt::overloaded_visit<const std::string&>(
      terminal.value, [&](const std::unique_ptr<AST::Variable>& variable) { return variable->name; },
      [&](const Token& token) { return token.value; },
      [&](const std::unique_ptr<AST::String>& string) { return string->value; });
}

auto Generator::get_identifier(const AST::Declaration& declaration) -> const std::string& {
  return declaration.name;
}

auto Generator::get_decl_type(const AST::Declaration& declaration) -> const std::string& {
  return declaration.type;
}

void Generator::init_context() {
  _contexts.emplace(Context{
      .size = 0,
      .stack = x86::Address{x86::Address::Mode::DIRECT, x86::Register::RSP, 0},
      .variables = {},
      .currVariable = 2,
      .typeIDs = {{"int", 0}, {"str", 1}},
      .types = {{0, Type_t{x86::Size::QWORD, std::nullopt}}, {1, Type_t{x86::Size::QWORD, std::nullopt}}}});
}

void Generator::init_data() { _output_data = "section   .data\n"; }
}  // namespace kuso