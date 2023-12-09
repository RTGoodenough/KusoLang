

#include "generator/generator.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

#include <belt/overload.hpp>
#include <variant>

#include "generator/context.hpp"
#include "lexer/token.hpp"
#include "linux/linux.hpp"
#include "x86/x86.hpp"

// TODO(rolland): add semantic errors function

namespace kuso {
void Generator::generate(const AST& ast) {
  std::cout << "generate\n";
  emit("global    _start\nsection   .text\n_start:\n");
  for (const auto& statement : ast) {
    generate(statement);
  }
}

void Generator::generate(const AST::Statement& statement) {
  std::cout << "generate\n";

  belt::overloaded_visit(
      statement.statement,
      [&](const std::unique_ptr<AST::Declaration>& declaration) { generate_declaration(*declaration); },
      [&](const std::unique_ptr<AST::Assignment>& assignment) { generate_assignment(*assignment); },
      [&](const std::unique_ptr<AST::Exit>& exit) { generate_exit(*exit); },
      [&](const std::unique_ptr<AST::Push>&) {}, [&](const std::unique_ptr<AST::Return>&) {},
      [](std::nullptr_t) {});
}

void Generator::generate_declaration(const AST::Declaration& declaration) {
  std::cout << "generate_declaration\n";
  auto&       current = context();
  const auto& name = get_identifier(declaration);
  auto        typeID = get_type_id(get_decl_type(declaration));

  if (current.variables.find(name) != current.variables.end()) {
    throw std::runtime_error("Multiple Declarations of " + name);
  }

  for (auto& variable : current.variables) {
    variable.second.location.disp += get_type(typeID).size;
  }

  current.variables[name] =
      Variable{typeID, {x86::Address::Mode::INDIRECT_DISPLACEMENT, x86::Register::RSP, 0}};
  emit(x86::Op::PUSH, x86::Literal{0});
  current.size += get_type(typeID).size;
  current.stack.disp += get_type(typeID).size;
}

void Generator::generate_assignment(const AST::Assignment& assignment) {
  std::cout << "generate_assignment\n";
  // TODO(rolland): check if assignment is valid

  if (std::holds_alternative<std::unique_ptr<AST::Declaration>>(assignment.dest)) {
    generate_declaration(*std::get<std::unique_ptr<AST::Declaration>>(assignment.dest));
  }

  const auto& name = get_identifier(assignment);

  auto variableIter = context().variables.find(name);
  if (variableIter == context().variables.end()) {
    throw std::runtime_error("Unknown Variable " + name);
  }

  // TODO(rolland): assuming this puts the value in rax
  generate_expression(*assignment.value);
  emit(x86::Op::MOV, variableIter->second.location, x86::Register::RAX);
}

void Generator::generate_expression(const AST::Expression& expression) {
  std::cout << "generate_expression\n";
  // TODO(rolland): implement
  belt::overloaded_visit(
      expression.value,
      [&](const std::unique_ptr<AST::Terminal>& terminal) { generate_expression(*terminal); },
      [&](const std::unique_ptr<AST::Declaration>&) {
        throw std::runtime_error("Declaration Expressions are not implemented");
      },
      [&](const std::unique_ptr<AST::BinaryExpression>&) {
        throw std::runtime_error("Binary Expressions are not implemented");
      },
      [&](const std::unique_ptr<AST::Push>&) {
        throw std::runtime_error("Push Expressions are not implemented");
      },
      [](std::nullptr_t) {});
}

void Generator::generate_expression(const AST::Terminal& terminal) {
  std::cout << "generate_expression\n";

  if (terminal.token.type == Token::Type::IDENTIFIER) {
    const auto& name = terminal.token.value;
    auto        variableIter = context().variables.find(name);
    if (variableIter == context().variables.end()) {
      throw std::runtime_error("Unknown Variable " + name);
    }

    // TODO(rolland): move address to rax instead of copying value
    emit(x86::Op::MOV, x86::Register::RAX, variableIter->second.location);
  } else if (terminal.token.type == Token::Type::NUMBER) {
    emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{std::stoi(terminal.token.value)});
  } else {
    throw std::runtime_error("Invalid Terminal");
  }
}

void Generator::generate_exit(const AST::Exit& exit) {
  std::cout << "generate_exit\n";
  if (exit.value) {
    generate_expression(*exit.value);
  } else {
    emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{0});
  }
  emit(x86::Op::MOV, x86::Register::RDI, x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::EXIT});
  emit(x86::Op::SYSCALL);
}

auto Generator::get_identifier(const AST::Expression& expression) -> const std::string& {
  std::cout << "get_identifier\n";
  return belt::overloaded_visit<const std::string&>(
      expression.value,
      [&](const std::unique_ptr<AST::Terminal>& terminal) -> const std::string& {
        return get_identifier(*terminal);
      },
      [&](const std::unique_ptr<AST::Declaration>& declaration) -> const std::string& {
        return get_identifier(*declaration);
      },
      [](const std::unique_ptr<AST::BinaryExpression>&) -> const std::string& {
        throw std::runtime_error("Invalid Expression");
      },
      [](const std::unique_ptr<AST::Push>&) -> const std::string& {
        throw std::runtime_error("Invalid Expression");
      },
      [](std::nullptr_t) -> const std::string& { throw std::runtime_error("Invalid Expression"); });
}

[[nodiscard]] auto Generator::get_identifier(const AST::Assignment& assignment) -> const std::string& {
  std::cout << "get_identifier\n";
  return belt::overloaded_visit<const std::string&>(
      assignment.dest,
      [](const std::unique_ptr<AST::Terminal>& terminal) -> const std::string& {
        return get_identifier(*terminal);
      },
      [](const std::unique_ptr<AST::Declaration>& declaration) -> const std::string& {
        return get_identifier(*declaration);
      },
      [](std::nullptr_t) -> const std::string& { throw std::runtime_error("Invalid Assignment"); });
}

auto Generator::get_identifier(const AST::Terminal& terminal) -> const std::string& {
  std::cout << "get_identifier\n";
  return terminal.token.value;
}

auto Generator::get_identifier(const AST::Declaration& declaration) -> const std::string& {
  std::cout << "get_identifier\n";
  return declaration.name->token.value;
}

auto Generator::get_decl_type(const AST::Declaration& declaration) -> const std::string& {
  std::cout << "get_decl_type\n";
  return declaration.type->token.value;
}

void Generator::emit(const std::string& value) { _outputFile.write_ln(value); }

void Generator::emit(x86::Op operation) { _outputFile.write_ln(x86::to_string(operation)); }

void Generator::emit(x86::Op operation, x86::Register src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(src));
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Address src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + dest.to_string() + ", " + src.to_string());
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Address src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(dest) + ", " + src.to_string());
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Register src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + dest.to_string() + ", " + x86::to_string(src));
}
void Generator::emit(x86::Op operation, x86::Address dest, x86::Literal value) {
  _outputFile.write_ln(x86::to_string(operation) + " " + dest.to_string() + ", " + value.to_string());
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Literal value) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(dest) + ", " + value.to_string());
}
void Generator::emit(x86::Op operation, x86::Register dest, x86::Register src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(dest) + ", " + x86::to_string(src));
}
void Generator::emit(x86::Op operation, x86::Literal value) {
  _outputFile.write_ln(x86::to_string(operation) + " " + value.to_string());
}

Generator::Generator(const std::filesystem::path& outputpath)
    : _outputFile(outputpath, std::ios_base::out | std::ios_base::trunc) {
  if (!_outputFile.is_open()) {
    throw std::runtime_error("Failed to open output file");
  }
  init_context();
}

auto Generator::get_type(int typeID) -> Type_t {
  std::cout << "get_type\n";
  auto typeIter = context().types.find(typeID);
  if (typeIter == context().types.end()) {
    throw std::runtime_error("Unknown Type " + std::to_string(typeID));
  }

  return typeIter->second;
}

auto Generator::get_type_id(const std::string& type) -> int {
  std::cout << "get_type_id\n";
  auto typeIter = context().typeIDs.find(type);
  if (typeIter == context().typeIDs.end()) {
    throw std::runtime_error("Unknown Type " + type);
  }

  return typeIter->second;
}

void Generator::init_context() {
  std::cout << "init_context\n";
  _contexts.emplace(Context{
      .size = 0,
      .stack = x86::Address{x86::Address::Mode::DIRECT, x86::Register::RSP, 0},
      .variables = {},
      .typeIDs =
          {
              {"int", 0},
              {"byte", 1},
          },
      .types =
          {
              {0, Type_t{x86::Size::QWORD}},
              {1, Type_t{1}},
          },
  });
}
}  // namespace kuso