#pragma once

#include <cstddef>
#include <memory>
#include <variant>
#include <vector>

#include <belt/class_macros.hpp>
#include <belt/overload.hpp>

#include "lexer/token.hpp"

namespace kuso {
class AST {
  DEFAULT_CONSTRUCTIBLE(AST)
  DEFAULT_DESTRUCTIBLE(AST)
  DEFAULT_MOVABLE(AST)
  NON_COPYABLE(AST)

 public:
  enum class BinaryOp {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW,
  };
  struct Terminal {
    Token token;
  };

  struct Declaration {
    std::unique_ptr<Terminal> name;
    std::unique_ptr<Terminal> type;
  };

  struct Expression;
  struct BinaryExpression {
    std::variant<std::unique_ptr<Expression>> left;
    std::variant<std::unique_ptr<Expression>> right;
    BinaryOp                                  op;

    [[nodiscard]] auto to_string() -> std::string {
      return belt::overloaded_visit(
                 left,
                 [](const std::unique_ptr<Expression>& expression) { return expression->to_string(); }) +
             to_char(op) + belt::overloaded_visit(right, [](const std::unique_ptr<Expression>& expression) {
               return expression->to_string();
             });
    }
  };

  struct Return {
    std::unique_ptr<Expression> value;
  };

  struct Push {
    std::unique_ptr<Expression> value;
    std::unique_ptr<Expression> dest;
  };

  struct Assignment {
    std::variant<std::unique_ptr<Terminal>, std::unique_ptr<Declaration>> dest;
    std::unique_ptr<Expression>                                           value;

    [[nodiscard]] auto to_string() -> std::string {
      return belt::overloaded_visit(
                 dest, [](const std::unique_ptr<Terminal>& terminal) { return terminal->token.value; },
                 [](const std::unique_ptr<Declaration>& declaration) {
                   return declaration->name->token.value + " as " + declaration->type->token.value;
                 }) +
             " = " + value->to_string() + '\n';
    }
  };

  struct Expression {
    enum class Type {
      L_VALUE,
      R_VALUE,
    };
    std::variant<std::unique_ptr<BinaryExpression>, std::unique_ptr<Terminal>, std::unique_ptr<Declaration>,
                 std::unique_ptr<Push>>
        value;

    [[nodiscard]] auto to_string() -> std::string {
      return belt::overloaded_visit(
          value,
          [](const std::unique_ptr<BinaryExpression>& binary_expression) {
            return binary_expression->to_string();
          },
          [](const std::unique_ptr<Terminal>& terminal) { return terminal->token.value; },
          [](const std::unique_ptr<Declaration>& declaration) {
            return declaration->name->token.value + " as " + declaration->type->token.value;
          },
          [](const std::unique_ptr<Push>& push) {
            return "push " + push->value->to_string() + " -> " + push->dest->to_string();
          });
    }
  };

  struct Statement {
    DEFAULT_CONSTRUCTIBLE(Statement)
    DEFAULT_DESTRUCTIBLE(Statement)
    DEFAULT_MOVABLE(Statement)
    NON_COPYABLE(Statement)

    std::variant<std::unique_ptr<Assignment>, std::unique_ptr<Push>, std::unique_ptr<Declaration>,
                 std::unique_ptr<Return>, std::nullptr_t>
        statement;

    explicit Statement(std::nullptr_t) : statement(nullptr) {}
    explicit Statement(std::unique_ptr<Push> push) : statement(std::move(push)) {}
    explicit Statement(std::unique_ptr<Return> return_) : statement(std::move(return_)) {}
    explicit Statement(std::unique_ptr<Assignment> assignment) : statement(std::move(assignment)) {}
    explicit Statement(std::unique_ptr<Declaration> declaration) : statement(std::move(declaration)) {}
  };

  void add_statement(Statement&& statement) { _statements.emplace_back(std::move(statement)); }

  [[nodiscard]] auto to_string() -> std::string {
    std::string result;
    for (const auto& statement : _statements) {
      belt::overloaded_visit(
          statement.statement,
          [&result](const std::unique_ptr<Assignment>& assignment) { result += assignment->to_string(); },
          [&result](const std::unique_ptr<Push>&) {
            // TODO(rolland): Implement push to string
            result += "push\n";
          },
          [&result](const std::unique_ptr<Return>& return_) {
            result += "return " + return_->value->to_string() + '\n';
          },
          [&result](const std::unique_ptr<Declaration>& declaration) {
            result +=
                "declare " + declaration->name->token.value + " as " + declaration->type->token.value + '\n';
          },
          [&result](std::nullptr_t) { result += "null\n"; });
    }
    return result;
  }

  static auto to_char(BinaryOp type) -> char {
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

 private:
  std::vector<Statement> _statements;
};
}  // namespace kuso