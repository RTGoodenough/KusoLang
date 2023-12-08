#pragma once

#include <cstddef>
#include <memory>
#include <variant>
#include <vector>

#include "lexer/token.hpp"

namespace kuso {
class AST {
  DEFAULT_CONSTRUCTIBLE(AST)
  DEFAULT_DESTRUCTIBLE(AST)
  DEFAULT_MOVABLE(AST)
  NON_COPYABLE(AST)

 public:
  enum class BinaryOp;
  struct Terminal;
  struct Declaration;
  struct BinaryExpression;
  struct Return;
  struct Push;
  struct Assignment;
  struct Expression;
  struct Statement;
  using iterator = std::vector<Statement>::iterator;
  using const_iterator = std::vector<Statement>::const_iterator;

  void               add_statement(Statement&&);
  [[nodiscard]] auto to_string() const -> std::string;

  [[nodiscard]] auto begin() -> iterator;
  [[nodiscard]] auto end() -> iterator;
  [[nodiscard]] auto begin() const -> const_iterator;
  [[nodiscard]] auto end() const -> const_iterator;

 private:
  std::vector<Statement> _statements;

  [[nodiscard]] static auto to_char(BinaryOp) -> char;
};

enum class AST::BinaryOp {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  POW,
};
struct AST::Terminal {
  Token token;
};

struct AST::Declaration {
  std::unique_ptr<Terminal> name;
  std::unique_ptr<Terminal> type;

  [[nodiscard]] auto to_string() const -> std::string;
};
struct AST::BinaryExpression {
  std::variant<std::unique_ptr<Expression>> left;
  std::variant<std::unique_ptr<Expression>> right;
  BinaryOp                                  op;

  [[nodiscard]] auto to_string() const -> std::string;
};

struct AST::Return {
  std::unique_ptr<Expression> value;
};

struct AST::Push {
  std::unique_ptr<Expression> value;
  std::unique_ptr<Expression> dest;

  [[nodiscard]] auto to_string() const -> std::string;
};

struct AST::Assignment {
  std::variant<std::unique_ptr<Terminal>, std::unique_ptr<Declaration>> dest;
  std::unique_ptr<Expression>                                           value;

  [[nodiscard]] auto to_string() const -> std::string;
};

struct AST::Expression {
  enum class Type {
    L_VALUE,
    R_VALUE,
  };

  std::variant<std::unique_ptr<BinaryExpression>, std::unique_ptr<Terminal>, std::unique_ptr<Declaration>,
               std::unique_ptr<Push>>
      value;

  [[nodiscard]] auto to_string() const -> std::string;
};

struct AST::Statement {
  std::variant<std::unique_ptr<Assignment>, std::unique_ptr<Push>, std::unique_ptr<Declaration>,
               std::unique_ptr<Return>, std::nullptr_t>
      statement;

  explicit Statement(std::nullptr_t) : statement(nullptr) {}
  explicit Statement(std::unique_ptr<Push> push) : statement(std::move(push)) {}
  explicit Statement(std::unique_ptr<Return> return_) : statement(std::move(return_)) {}
  explicit Statement(std::unique_ptr<Assignment> assignment) : statement(std::move(assignment)) {}
  explicit Statement(std::unique_ptr<Declaration> declaration) : statement(std::move(declaration)) {}
};
}  // namespace kuso