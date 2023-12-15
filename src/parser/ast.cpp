
#include "parser/ast.hpp"

#include <fmt/format.h>
#include <belt/class_macros.hpp>
#include <belt/overload.hpp>

namespace kuso {
auto AST::to_string() const -> std::string {
  std::string result;
  for (const auto& statement : _statements) {
    belt::overloaded_visit(
        statement.statement,
        [&result](const std::unique_ptr<Assignment>& assignment) { result += assignment->to_string(0); },
        [&result](const std::unique_ptr<Push>&) {
          // TODO(rolland): Implement push to string
          result += "push\n";
        },
        [&result](const std::unique_ptr<If>& if_) { result += if_->to_string(0); },
        [&result](const std::unique_ptr<Print>& print) { result += print->value->to_string(0); },
        [&result](const std::unique_ptr<Return>& return_) { result += return_->value->to_string(0); },
        [&result](const std::unique_ptr<Exit>& exit) { result += exit->value->to_string(0); },
        [&result](const std::unique_ptr<Declaration>& declaration) { result += declaration->to_string(0); },
        [&result](std::nullptr_t) { result += "null\n"; });
  }
  return result;
}

void AST::add_statement(Statement&& statement) { _statements.emplace_back(std::move(statement)); }

auto AST::op_to_string(BinaryOp type) -> std::string {
  static const std::unordered_map<BinaryOp, std::string> OPERATORS{
      {BinaryOp::ADD, "+"}, {BinaryOp::SUB, "-"}, {BinaryOp::MUL, "*"},
      {BinaryOp::DIV, "/"}, {BinaryOp::MOD, "%"}, {BinaryOp::POW, "^"},
  };
  auto oper = OPERATORS.find(type);
  if (oper == OPERATORS.end()) {
    // TODO(rolland): handle this error
    return "!";
  }

  return oper->second;
}

auto AST::begin() -> iterator { return _statements.begin(); }
auto AST::end() -> iterator { return _statements.end(); }
auto AST::begin() const -> const_iterator { return _statements.begin(); }
auto AST::end() const -> const_iterator { return _statements.end(); }

// ----------------------------------------- NODE TYPES ------------------------------------------------------

auto AST::Assignment::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
             dest,
             [&](const std::unique_ptr<Terminal>& terminal) {
               return fmt::format("\n{: >{}}", "", indent) + terminal->token.value;
             },
             [&](const std::unique_ptr<Declaration>& declaration) {
               return fmt::format("\n{: >{}}", "", indent) + declaration->name->token.value + " as " +
                      declaration->type->token.value;
             }) +
         " = " + value->to_string(indent + 1) + '\n';
}

auto AST::Expression::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Expression:", "", indent) + value->to_string(indent + 1);
}

auto AST::Declaration::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Declaration:", "", indent) + name->token.value + " as " + type->token.value;
}

auto AST::Push::to_string(int) const -> std::string { return "push "; }

auto AST::Return::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Return:", "", indent) + value->to_string(indent + 1);
}

auto AST::Exit::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Exit:", "", indent) + value->to_string(indent + 1);
}

auto AST::Print::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Print:", "", indent) + value->to_string(indent + 1);
}

auto AST::If::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}", "", indent) + condition->to_string(indent + 1) + ":\n";
}

auto AST::Comparison::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Comparison", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

auto AST::Equality::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Equality:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? (equal ? "== " : "!= ") + right->to_string(indent + 1) : "");
}

auto AST::Term::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Term:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

auto AST::Factor::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Factor:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

auto AST::Unary::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      value,
      [&](const std::unique_ptr<Unary>& unary) {
        return fmt::format("\n{: >{}}Unary:", "", indent) + op_to_string(op) + unary->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Primary>& value) {
        return fmt::format("\n{: >{}}Unary:", "", indent) + value->to_string(indent + 1);
      });
}

auto AST::Primary::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      value,
      [&](const std::unique_ptr<Expression>& expression) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + expression->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Terminal>& terminal) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + terminal->token.value;
      },
      [&](const std::unique_ptr<String>& declaration) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + declaration->value;
      });
}
}  // namespace kuso
