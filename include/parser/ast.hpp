/**
 * @file ast.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "lexer/token.hpp"

namespace kuso {
/**
 * @brief Abstract Syntax Tree class
 * 
 */
class AST {
  DEFAULT_CONSTRUCTIBLE(AST)
  DEFAULT_DESTRUCTIBLE(AST)
  DEFAULT_MOVABLE(AST)
  NON_COPYABLE(AST)

 public:
  /**
   * @brief Binary operations
   * 
   */
  enum class BinaryOp {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW,
    EQ,
    NEQ,
    LT,
    GT,
    LTE,
    GTE,
    NOT,
  };

  struct Declaration;
  struct Return;
  struct Push;
  struct Print;
  struct Assignment;

  struct Expression;
  struct Equality;
  struct Comparison;
  struct Term;
  struct Factor;
  struct Unary;
  struct Primary;
  struct Terminal;
  struct Variable;

  struct Type;
  struct Attribute;

  struct Statement;
  struct Exit;
  struct String;
  struct If;

  struct While;

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

  [[nodiscard]] static auto op_to_string(BinaryOp) -> std::string;
};

/**
 * @brief AST node for string literals
 * 
 */
struct AST::String {
  std::string value;
};

/**
 * @brief AST node for exit statements
 * 
 */
struct AST::Exit {
  std::unique_ptr<Expression> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for declarations
 * 
 */
struct AST::Declaration {
  std::string                 name;
  std::string                 type;
  std::unique_ptr<Expression> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for return statements
 * 
 */
struct AST::Return {
  std::unique_ptr<Expression> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for push statements
 * 
 */
struct AST::Push {
  std::unique_ptr<Expression> value;
  std::unique_ptr<Expression> dest;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for assignments
 * 
 */
struct AST::Assignment {
  std::unique_ptr<Variable>   dest;
  std::unique_ptr<Expression> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for print statements
 * 
 */
struct AST::Print {
  std::unique_ptr<Expression> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for expressions
 * 
 */
struct AST::Expression {
  std::unique_ptr<Equality> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for equality expressions
 * 
 */
struct AST::Equality {
  std::unique_ptr<Comparison> left;
  std::unique_ptr<Equality>   right;
  bool                        equal;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for comparison expressions
 * 
 */
struct AST::Comparison {
  std::unique_ptr<Term>       left;
  std::unique_ptr<Comparison> right;
  BinaryOp                    op;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for term expressions
 * 
 */
struct AST::Term {
  std::unique_ptr<Factor> left;
  std::unique_ptr<Term>   right;
  BinaryOp                op;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for factor expressions
 * 
 */
struct AST::Factor {
  std::unique_ptr<Unary>  left;
  std::unique_ptr<Factor> right;
  BinaryOp                op;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for unary expressions
 * 
 */
struct AST::Terminal {
  std::variant<std::unique_ptr<Variable>, Token, std::unique_ptr<String>> value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for unary expressions
 * 
 */
struct AST::Unary {
  std::variant<std::unique_ptr<Unary>, std::unique_ptr<Primary>> value;
  BinaryOp                                                       op;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for primary expressions
 * 
 */
struct AST::Primary {
  std::variant<std::unique_ptr<Variable>, std::unique_ptr<Terminal>, std::unique_ptr<Expression>,
               std::unique_ptr<String>>
      value;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for variable/attribute access
 * 
 */
struct AST::Variable {
  std::string                name;
  std::optional<std::string> attribute;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for if statements
 * 
 */
struct AST::If {
  std::unique_ptr<Expression> condition;
  std::vector<Statement>      body;
  std::vector<Statement>      elseBody;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for type definitions
 * 
 */
struct AST::Attribute {
  std::string name;
  std::string type;
};

/**
 * @brief AST node for while statements
 * 
 */
struct AST::While {
  std::unique_ptr<Expression> condition;
  std::vector<Statement>      body;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for type definitions
 * 
 */
struct AST::Type {
  std::string            name;
  std::vector<Attribute> attributes;

  [[nodiscard]] auto to_string(int) const -> std::string;
};

/**
 * @brief AST node for statements
 * 
 */
struct AST::Statement {
  std::variant<std::unique_ptr<Type>, std::unique_ptr<If>, std::unique_ptr<Exit>, std::unique_ptr<Assignment>,
               std::unique_ptr<Push>, std::unique_ptr<Declaration>, std::unique_ptr<Return>,
               std::unique_ptr<Print>, std::unique_ptr<While>, std::nullptr_t>
      statement;

  [[nodiscard]] auto to_string(int) const -> std::string;

  explicit Statement(std::nullptr_t) : statement(nullptr) {}
  explicit Statement(std::unique_ptr<Exit> exit) : statement(std::move(exit)) {}
  explicit Statement(std::unique_ptr<Push> push) : statement(std::move(push)) {}
  explicit Statement(std::unique_ptr<Return> return_) : statement(std::move(return_)) {}
  explicit Statement(std::unique_ptr<Assignment> assignment) : statement(std::move(assignment)) {}
  explicit Statement(std::unique_ptr<Declaration> declaration) : statement(std::move(declaration)) {}
  explicit Statement(std::unique_ptr<Type> type) : statement(std::move(type)) {}
  explicit Statement(std::unique_ptr<Print> print) : statement(std::move(print)) {}
  explicit Statement(std::unique_ptr<If> if_) : statement(std::move(if_)) {}
  explicit Statement(std::unique_ptr<While> while_) : statement(std::move(while_)) {}
};
}  // namespace kuso