

#include "parser/ast.hpp"

#include <belt/class_macros.hpp>
#include <belt/overload.hpp>

namespace kuso {
auto AST::to_string() const -> std::string {
  std::string result;
  for (const auto& statement : _statements) {
    belt::overloaded_visit(
        statement.statement,
        [&result](const std::unique_ptr<Assignment>& assignment) { result += assignment->to_string(); },
        [&result](const std::unique_ptr<Push>&) {
          // TODO(rolland): Implement push to string
          result += "push\n";
        },
        [&result](const std::unique_ptr<If>& if_) { result += if_->to_string(); },
        [&result](const std::unique_ptr<Print>& print) {
          result += "print " + print->value->to_string() + '\n';
        },
        [&result](const std::unique_ptr<Return>& return_) {
          result += "return " + return_->value->to_string() + '\n';
        },
        [&result](const std::unique_ptr<Exit>& exit) { result += "exit " + exit->value->to_string() + '\n'; },
        [&result](const std::unique_ptr<Declaration>& declaration) {
          result +=
              "declare " + declaration->name->token.value + " as " + declaration->type->token.value + '\n';
        },
        [&result](std::nullptr_t) { result += "null\n"; });
  }
  return result;
}

void AST::add_statement(Statement&& statement) { _statements.emplace_back(std::move(statement)); }

auto AST::to_char(BinaryOp type) -> char {
  static const std::unordered_map<BinaryOp, char> OPERATORS{
      {BinaryOp::ADD, '+'}, {BinaryOp::SUB, '-'}, {BinaryOp::MUL, '*'},
      {BinaryOp::DIV, '/'}, {BinaryOp::MOD, '%'}, {BinaryOp::POW, '^'},
  };
  auto oper = OPERATORS.find(type);
  if (oper == OPERATORS.end()) {
    return '!';
  }

  return oper->second;
}

auto AST::begin() -> iterator { return _statements.begin(); }
auto AST::end() -> iterator { return _statements.end(); }
auto AST::begin() const -> const_iterator { return _statements.begin(); }
auto AST::end() const -> const_iterator { return _statements.end(); }

// ----------------------------------------- NODE TYPES ------------------------------------------------------

auto AST::BinaryExpression::to_string() const -> std::string {
  return belt::overloaded_visit<std::string>(
             left, [](const std::unique_ptr<Expression>& expression) { return expression->to_string(); }) +
         to_char(op) +
         belt::overloaded_visit<std::string>(
             right, [](const std::unique_ptr<Expression>& expression) { return expression->to_string(); });
}

auto AST::Assignment::to_string() const -> std::string {
  return belt::overloaded_visit<std::string>(
             dest, [](const std::unique_ptr<Terminal>& terminal) { return terminal->token.value; },
             [](const std::unique_ptr<Declaration>& declaration) {
               return declaration->name->token.value + " as " + declaration->type->token.value;
             }) +
         " = " + value->to_string() + '\n';
}

auto AST::Expression::to_string() const -> std::string {
  return belt::overloaded_visit<std::string>(
      value,
      [](const std::unique_ptr<BinaryExpression>& binary_expression) {
        return binary_expression->to_string();
      },
      [](const AST::String& string) { return string.value; },
      [](const std::unique_ptr<Terminal>& terminal) { return terminal->token.value; },
      [](const std::unique_ptr<Declaration>& declaration) {
        return declaration->name->token.value + " as " + declaration->type->token.value;
      },
      [](const std::unique_ptr<Push>& push) {
        return "push " + push->value->to_string() + " -> " + push->dest->to_string();
      });
}

auto AST::Declaration::to_string() const -> std::string {
  return name->token.value + " as " + type->token.value;
}

auto AST::Push::to_string() const -> std::string {
  return "push " + value->to_string() + " -> " + dest->to_string();
}

auto AST::Return::to_string() const -> std::string { return "return " + value->to_string(); }

auto AST::Exit::to_string() const -> std::string { return "exit " + value->to_string(); }

auto AST::Print::to_string() const -> std::string { return "print " + value->to_string(); }

auto AST::If::to_string() const -> std::string {
  std::string result = "if " + condition->to_string() + ":\n";
  return result;
}

}  // namespace kuso
