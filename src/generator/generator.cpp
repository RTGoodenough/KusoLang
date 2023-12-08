

#include "generator/generator.hpp"

#include <belt/overload.hpp>
#include <memory>
#include "generator/context.hpp"
#include "x86/x86.hpp"

namespace kuso {
void Generator::generate(const AST& ast) {
  for (const auto& statement : ast) {
    generate(statement);
  }
}

void Generator::generate(const AST::Statement& statement) {
  belt::overloaded_visit(
      statement.statement,
      [&](const std::unique_ptr<AST::Declaration>& declaration) { generate_declaration(*declaration); },
      [&](const std::unique_ptr<AST::Assignment>&) {

      },
      [&](const std::unique_ptr<AST::Push>&) {}, [&](const std::unique_ptr<AST::Return>&) {},
      [](std::nullptr_t) {});
}

void Generator::generate_declaration(const AST::Declaration& declaration) {
  const auto& name = get_identifier(declaration);
  const auto& type = get_type(declaration);

  if (context().variables.find(name) != context().variables.end()) {
    throw std::runtime_error("Multiple Declarations of " + name);
  }

  auto typeIter = context().types.find(type);
  if (typeIter != context().types.end()) {
    throw std::runtime_error("Unknown Type " + type);
  }

  context().variables.emplace(name, typeIter->second);
}

void Generator::generate_assignment(const AST::Assignment& assignment) {
  // TODO
}

auto Generator::get_identifier(const AST::Expression& expression) -> const std::string& {
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

auto Generator::get_identifier(const AST::Terminal& terminal) -> const std::string& {
  return terminal.token.value;
}

auto Generator::get_identifier(const AST::Declaration& declaration) -> const std::string& {
  return declaration.name->token.value;
}

auto Generator::get_type(const AST::Declaration& declaration) -> const std::string& {
  return declaration.type->token.value;
}

void Generator::emit(const std::string& value) { _outputFile.write_ln(value); }
void Generator::emit(x86::Op operation) { _outputFile.write_ln(x86::to_string(operation)); }
void Generator::emit(x86::Op operation, x86::Register src) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(src));
}
void Generator::emit(x86::Op operation, x86::Source src, x86::Destination dest) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(src) + ", " + x86::to_string(dest));
}
void Generator::emit(x86::Op operation, x86::Source src, int dest) {
  _outputFile.write_ln(x86::to_string(operation) + " " + x86::to_string(src) + ", " + std::to_string(dest));
}

}  // namespace kuso