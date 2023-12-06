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
  DEFAULT_COPYABLE(AST)
  DEFAULT_DESTRUCTIBLE(AST)
  DEFAULT_MOVABLE(AST)
 public:
  enum class Operator {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW,
  };

  enum class BinaryOp {
    AND,
    OR,
    XOR,
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
  };

  struct Push {
    std::unique_ptr<Expression> value;
    std::unique_ptr<Expression> dest;
  };

  struct Expression {
    enum class Type {
      L_VALUE,
      R_VALUE,
    };
    std::variant<std::unique_ptr<BinaryExpression>, std::unique_ptr<Terminal>, std::unique_ptr<Declaration>,
                 std::unique_ptr<Push>>
        value;
  };

  struct Statement {
    std::variant<std::unique_ptr<Push>, std::unique_ptr<Declaration>, std::nullptr_t> statement;

    explicit Statement(std::nullptr_t) : statement(nullptr) {}
    explicit Statement(std::unique_ptr<Push> push) : statement(std::move(push)) {}
    explicit Statement(std::unique_ptr<Declaration> declaration) : statement(std::move(declaration)) {}
  };

  void add_statement(Statement&& statement) { _statements.emplace_back(std::move(statement)); }

  [[nodiscard]] auto to_string() -> std::string {
    std::string result;
    for (const auto& statement : _statements) {
      std::visit(belt::overload{
                     [&result](const std::unique_ptr<Push>&) {
                       //  result += "push " + push->value + " -> " + push->dest + '\n';
                       result += "push\n";
                     },
                     [&result](const std::unique_ptr<Declaration>& declaration) {
                       result += "declare " + declaration->name->token.value + " as " +
                                 declaration->type->token.value + '\n';
                     },
                     [&result](std::nullptr_t) { result += "null\n"; },
                 },
                 statement.statement);
    }
    return result;
  }

 private:
  std::vector<Statement> _statements;
};
}  // namespace kuso