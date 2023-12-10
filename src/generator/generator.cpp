

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
#include "x86/x86.hpp"

// TODO(rolland): add semantic errors function

namespace kuso {
void Generator::generate(const AST& ast) {
  emit("global    _start\nsection   .text\n_start:\n");
  for (const auto& statement : ast) {
    generate(statement);
  }

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
  // TODO(rolland): implement
  belt::overloaded_visit(
      expression.value,
      [&](const std::unique_ptr<AST::Terminal>& terminal) { generate_expression(*terminal); },
      [&](const AST::String& string) { generate_string(string); },
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
  if (exit.value) {
    generate_expression(*exit.value);
  } else {
    emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{0});
  }
  emit(x86::Op::MOV, x86::Register::RDI, x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::EXIT});
  emit(x86::Op::SYSCALL);
}

void Generator::generate_print(const AST::Print& print) {
  generate_expression(*print.value);
  emit(x86::Op::MOV, x86::Register::RSI, x86::Register::RAX);
  emit(x86::Op::MOV, x86::Register::RAX, x86::Literal{lnx::Syscall::WRITE});
  emit(x86::Op::MOV, x86::Register::RDI, x86::Literal{1});
  emit(x86::Op::SYSCALL);
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

auto Generator::generate_if(const AST::If& ifStatement) -> void {
  generate_expression(*ifStatement.condition);
  emit(x86::Op::CMP, x86::Register::RAX, x86::Literal{0});
  auto label = fmt::format("if_{}", context().labels.size());
  context().labels.push(label);
  emit(x86::Op::JE, label);
  for (const auto& statement : ifStatement.body) {
    generate(statement);
  }
  emit(label + ":");
  context().labels.pop();
}

[[nodiscard]] auto Generator::get_identifier(const AST::Expression& expression) -> const std::string& {
  return belt::overloaded_visit<const std::string&>(
      expression.value,
      [&](const std::unique_ptr<AST::Terminal>& terminal) -> const std::string& {
        return get_identifier(*terminal);
      },
      [&](const std::unique_ptr<AST::Declaration>& declaration) -> const std::string& {
        return get_identifier(*declaration);
      },
      [](const AST::String&) -> const std::string& { throw std::runtime_error("Invalid Expression"); },
      [](const std::unique_ptr<AST::BinaryExpression>&) -> const std::string& {
        throw std::runtime_error("Invalid Expression");
      },
      [](const std::unique_ptr<AST::Push>&) -> const std::string& {
        throw std::runtime_error("Invalid Expression");
      },
      [](std::nullptr_t) -> const std::string& { throw std::runtime_error("Invalid Expression"); });
}

[[nodiscard]] auto Generator::get_identifier(const AST::Assignment& assignment) -> const std::string& {
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
  return terminal.token.value;
}

auto Generator::get_identifier(const AST::Declaration& declaration) -> const std::string& {
  return declaration.name->token.value;
}

auto Generator::get_decl_type(const AST::Declaration& declaration) -> const std::string& {
  return declaration.type->token.value;
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

auto Generator::get_type(int typeID) -> Type_t {
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

void Generator::init_context() {
  _contexts.emplace(Context{
      .size = 0,
      .stack = x86::Address{x86::Address::Mode::DIRECT, x86::Register::RSP, 0},
      .variables = {},
      .typeIDs = {{"int", 0}, {"byte", 1}, {"str", 2}},
      .types = {{0, Type_t{x86::Size::QWORD}}, {1, Type_t{1}}, {2, Type_t{x86::Size::QWORD}}},
      .labels = {},
  });
}

void Generator::init_data() { _output_data = "section   .data\n"; }
}  // namespace kuso