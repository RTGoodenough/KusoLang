/**
 * @file generator.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include "generator/generator.hpp"

#include <cstdio>
#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>
#include <variant>

#include <belt/overload.hpp>

#include "generator/context.hpp"
#include "lexer/token.hpp"
#include "linux/linux.hpp"
#include "logging/logging.hpp"
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
  try {
    if (!_firstpass.types_pass(ast)) return;
    if (!_firstpass.function_pass(ast)) return;

    emit("global _start\nsection .text\n");
    for (const auto& statement : ast) {
      generate(statement);
    }

    _outputFile.write(_output_code);
  } catch (std::exception& e) {
    Logging::error(e.what());
  }
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
      [&](const std::unique_ptr<AST::Main>& main) { generate_main(*main); },
      [&](const std::unique_ptr<AST::Type>&) {},
      [&](const std::unique_ptr<AST::While>& whileStatement) { generate_while(*whileStatement); },
      [&](const std::unique_ptr<AST::Func>& func) { generate_func(*func); },
      [&](const std::unique_ptr<AST::Call>& call) { generate_call(*call); },
      [&](const std::unique_ptr<AST::Return>& return_) { generate_return(*return_); }, [](std::nullptr_t) {});
}

/**
 * @brief Generates x64 assembly from a declaration
 * 
 * @param declaration Declaration to generate from
 */
void Generator::generate_declaration(const AST::Declaration& declaration) {
  auto& current = context();

  auto typeID = _firstpass.get_type_id(declaration.type);
  if (!typeID.has_value()) {
    throw std::runtime_error("Unknown Type " + declaration.type);
  }

  auto type = _firstpass.get_type(typeID.value());
  if (!type.has_value()) {
    throw std::runtime_error("Unknown Type " + declaration.type);
  }
  const auto& typeRef = type.value().get();

  const auto& func = get_check_func_info(_currentFunction.top());
  if (declaration.value) {
    if (typeRef.offsets) throw std::runtime_error("Cannot assign value to type with attributes");
    generate_expression(*declaration.value);
    emit(x64::Op::MOV, func.locals.at(declaration.name).location, x64::Register::RAX);
  }

  current.variables[declaration.name] = Variable{typeID.value(), func.locals.at(declaration.name).location};
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
  emit(x64::Op::MOV, get_location(*assignment.dest), x64::Register::RAX);
}

void Generator::generate_main(const AST::Main& main) {
  _currentFunction.emplace("main");

  emit("_start:");
  enter_context("main");
  for (const auto& statement : main.body) {
    generate(statement);
  }
}

void Generator::generate_func(const AST::Func& func) {
  auto funcIter = _functions.find(func.name);
  if (funcIter != _functions.end()) {
    throw std::runtime_error("Multiple Declarations of " + func.name);
  }

  _functions.emplace(func.name, Function{.label = fmt::format(".func_{}", _functions.size()),
                                         .body = std::cref(func),
                                         .argCnt = func.args.size()});

  const auto& label = _functions.at(func.name).label;
  emit(fmt::format("{}:", label));
  _currentFunction.push(func.name);
  enter_context(func.name);

  for (const auto& statement : func.body) {
    generate(statement);
  }

  _currentFunction.pop();
}

void Generator::generate_call(const AST::Call& call) {
  auto funcIter = _functions.find(call.name);
  if (funcIter == _functions.end()) {
    throw std::runtime_error("Unknown Function " + call.name);
  }
  const auto& func = funcIter->second;

  if (call.args.size() != func.argCnt) {
    throw std::runtime_error("Invalid number of arguments for " + call.name);
  }

  generate_parameters(call);
  emit(x64::Op::CALL, func.label);
}

void Generator::generate_parameters(const AST::Call& call) {
  size_t paramIndex = 0;
  for (const auto& arg : call.args) {
    generate_expression(*arg);
    auto reg = x64::parameter_reg(paramIndex);
    if (reg == x64::Register::NONE) {
      push(x64::Register::RAX);
    } else {
      emit(x64::Op::MOV, reg, x64::Register::RAX);
    }
    ++paramIndex;
  }
}

void Generator::generate_return(const AST::Return& ret) {
  if (ret.value) {
    generate_expression(*ret.value);
  }
  leave_context();
  emit(x64::Op::RET);
}

/**
 * @brief Generates x64 assembly from an expression
 * 
 * @param expression Expression to generate from
 */
void Generator::generate_expression(const AST::Expression& expression) {
  generate_expression(*expression.value);
  if (!_exprInReg) pop(x64::Register::RAX);
}

/**
 * @brief Generates x64 assembly from an equality expression
 * 
 * @param equality Equality to generate from
 */
void Generator::generate_expression(const AST::Equality& equality) {
  if (equality.right) {
    generate_expression(*equality.right);
    if (_exprInReg) push(x64::Register::RAX);
  }
  generate_expression(*equality.left);

  if (equality.right) {
    if (!_exprInReg) pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    emit(x64::Op::CMP, x64::Register::RAX, x64::Register::RDX);
    pull_comparison_result(equality.equal ? AST::BinaryOp::EQ : AST::BinaryOp::NEQ);
    _exprInReg = true;
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
    if (_exprInReg) push(x64::Register::RAX);
  }
  generate_expression(*comparison.left);

  if (comparison.right) {
    if (!_exprInReg) pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    emit(x64::Op::CMP, x64::Register::RAX, x64::Register::RDX);
    pull_comparison_result(comparison.op);
    _exprInReg = true;
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
    if (_exprInReg) push(x64::Register::RAX);
  }
  generate_expression(*term.left);

  if (term.right) {
    if (!_exprInReg) pop(x64::Register::RAX);
    pop(x64::Register::RDX);
    if (term.op == AST::BinaryOp::ADD)
      emit(x64::Op::ADD, x64::Register::RAX, x64::Register::RDX);
    else if (term.op == AST::BinaryOp::SUB)
      emit(x64::Op::SUB, x64::Register::RAX, x64::Register::RDX);
    _exprInReg = true;
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
    if (_exprInReg) push(x64::Register::RAX);
  }
  generate_expression(*factor.left);

  if (factor.right) {
    if (!_exprInReg) pop(x64::Register::RAX);
    if (factor.op == AST::BinaryOp::MUL) {
      pop(x64::Register::RDX);
      emit(x64::Op::IMUL, x64::Register::RAX, x64::Register::RDX);
    } else if (factor.op == AST::BinaryOp::DIV) {
      emit(x64::Op::XOR, x64::Register::RDX, x64::Register::RDX);
      pop(x64::Register::RCX);
      emit(x64::Op::IDIV, x64::Register::RCX);
    }
    _exprInReg = true;
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
    if (!_exprInReg) pop(x64::Register::RAX);
    emit(x64::Op::NEG, x64::Register::RAX);
    _exprInReg = true;
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
      [&](const std::unique_ptr<AST::Call>& call) { generate_call(*call); },
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
  auto& current = context();

  auto variableIter = current.variables.find(variable.name);
  if (variableIter == current.variables.end()) {
    throw std::runtime_error("Unknown Variable " + variable.name);
  }
  emit(x64::Op::MOV, x64::Register::RAX, variableIter->second.location);
  _exprInReg = true;
}

/**
 * @brief Generates x64 assembly from a token
 * 
 * @param token Token to generate from
 */
void Generator::generate_expression(const Token& token) {
  if (token.type == Token::Type::NUMBER) {
    emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{std::stoi(token.value)});
    _exprInReg = true;
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
  emit(x64::Op::MOV, x64::Register::RSI, x64::Register::RAX);
  emit(x64::Op::MOV, x64::Register::RAX, x64::Literal{lnx::Syscall::WRITE});
  emit(x64::Op::MOV, x64::Register::RDI, x64::Literal{1});
  emit(x64::Op::SYSCALL);
}

/**
 * @brief Generates x64 assembly from a string
 * 
 * @param string String to generate from
 */
//NOLINTNEXTLINE
void Generator::generate_string(const AST::String&) { throw std::runtime_error("Strings Not Implemented"); }

/**
 * @brief Generates x64 assembly from an if statement
 * 
 * @param ifNode If to generate from
 */
auto Generator::generate_if(const AST::If& ifNode) -> void {
  generate_expression(*ifNode.condition);
  emit(x64::Op::CMP, x64::Register::RAX, x64::Literal{0});

  auto elseLabel = new_label();
  auto endLabel = new_label();

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
  auto startLabel = new_label();
  auto endLabel = new_label();

  emit(fmt::format("{}:", startLabel));
  generate_expression(*whileStatement.condition);
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

auto Generator::get_check_type(const std::string& typeName) -> Type& {
  auto type = _firstpass.get_type(typeName);
  if (!type.has_value()) {
    throw std::runtime_error("Unknown Type " + typeName);
  }

  return type.value().get();
}

auto Generator::get_check_type(TypeID typeId) -> Type& {
  auto type = _firstpass.get_type(typeId);
  if (!type.has_value()) {
    throw std::runtime_error("Unknown Type");
  }

  return type.value().get();
}

auto Generator::get_check_func_info(const std::string& funcname) -> const FirstPass::FuncInfo& {
  auto func = _firstpass.get_function(funcname);
  if (!func.has_value()) {
    throw std::runtime_error("Unknown Function " + funcname);
  }

  return func.value().get();
}

/**
 * @brief Creates a new context
 * 
 */
void Generator::enter_context(int64_t size) {
  auto& current = context();
  current = _contexts.emplace(Context{.size = 0,
                                      .stack = current.stack,
                                      .variables = current.variables,
                                      .currVariable = current.currVariable});

  for (int64_t i = 0; i < size / x64::Size::QWORD; ++i) {
    push(x64::Literal{0});
  }
}

void Generator::enter_context(const std::string& funcname) {
  const auto& func = get_check_func_info(funcname);
  auto&       current = _contexts.emplace(
            Context{.size = 0,
                    .stack = x64::Address{x64::Address::Mode::INDIRECT_DISPLACEMENT, x64::Register::RSP, 0},
                    .variables = {},
                    .currVariable = 0});

  for (const auto& arg : func.params) {
    if (arg.second.location.reg != x64::Register::RSP) {
      if (func.dirtyRegs.at(static_cast<size_t>(arg.second.location.reg))) {
        push(arg.second.location.reg);
        current.variables[arg.first] = arg.second;
        current.variables[arg.first].location = current.stack;
      } else {
        current.variables[arg.first].location = arg.second.location;
      }
    }
  }

  for (const auto& arg : func.locals) {
    push(x64::Literal{0});
    current.variables[arg.first] = arg.second;
    current.variables[arg.first].location = current.stack;
  }
}

/**
 * @brief Removes the current context
 * 
 */
void Generator::leave_context() {
  std::for_each(context().variables.begin(), context().variables.end(), [&](const auto& var) {
    if (var.second.location.reg == x64::Register::RSP) {
      pop(x64::Register::RDI);
    }
  });
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
  current.stack.disp += x64::Size::QWORD;
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

void Generator::emit(x64::Op operation, x64::Literal value1, x64::Literal value2) {
  _output_code.append(
      fmt::format("{} {}, {}\n", x64::to_string(operation), value1.to_string(), value2.to_string()));
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

auto Generator::new_label() -> std::string {
  auto label = fmt::format("label_{}", _label_count);
  ++_label_count;
  return label;
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

Generator::Generator(const std::filesystem::path& outputpath)
    : _outputFile(outputpath, std::ios_base::out | std::ios_base::trunc) {
  if (!_outputFile.is_open()) {
    throw std::runtime_error("Failed to open output file");
  }
  init_context();
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
    const auto& type = get_check_type(variableIter->second.type);
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
      terminal.value,
      [&](const std::unique_ptr<AST::Variable>& variable) -> const std::string& { return variable->name; },
      [&](const Token& token) -> const std::string& { return token.value; },
      [&](const std::unique_ptr<AST::String>& str) -> const std::string& { return str->value; });
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
  _contexts.emplace(
      Context{.size = 0,
              .stack = x64::Address{x64::Address::Mode::INDIRECT_DISPLACEMENT, x64::Register::RSP, 0},
              .variables = {},
              .currVariable = 0});
}
}  // namespace kuso