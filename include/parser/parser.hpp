/**
 * @file parser.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <initializer_list>
#include <optional>

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"

namespace kuso {
/**
  * @brief Parser class
  * 
  */
class Parser {
  DEFAULT_CONSTRUCTIBLE(Parser)
  DEFAULT_COPYABLE(Parser)
  DEFAULT_DESTRUCTIBLE(Parser)
  DEFAULT_MOVABLE(Parser)

  using Tokens = belt::Generator<Token>;

 public:
  [[nodiscard]] auto parse(const std::filesystem::path&) -> std::optional<AST>;
  [[nodiscard]] auto parse(const std::vector<Token>&) -> std::optional<AST>;

  struct ParseError : public std::runtime_error {
    explicit ParseError(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  Lexer _lexer;
  Token _lookahead;

  auto try_match(std::initializer_list<Token::Type>, Token&, Tokens&) -> bool;
  void match(std::initializer_list<Token::Type>, Token&, Tokens&);
  auto consume(Tokens&) -> Token;

  [[noreturn]] static void syntax_error(const Token&, const Token&);

  [[nodiscard]] auto parse_statement(Token&, Tokens&) -> AST::Statement;
  [[nodiscard]] auto parse_exit(Token&, Tokens&) -> std::unique_ptr<AST::Exit>;

  [[nodiscard]] auto parse_expression(Token&, Tokens&) -> std::unique_ptr<AST::Expression>;
  [[nodiscard]] auto parse_equality(Token&, Tokens&) -> std::unique_ptr<AST::Equality>;
  [[nodiscard]] auto parse_comparison(Token&, Tokens&) -> std::unique_ptr<AST::Comparison>;
  [[nodiscard]] auto parse_term(Token&, Tokens&) -> std::unique_ptr<AST::Term>;
  [[nodiscard]] auto parse_factor(Token&, Tokens&) -> std::unique_ptr<AST::Factor>;
  [[nodiscard]] auto parse_unary(Token&, Tokens&) -> std::unique_ptr<AST::Unary>;
  [[nodiscard]] auto parse_primary(Token&, Tokens&) -> std::unique_ptr<AST::Primary>;
  [[nodiscard]] auto parse_variable(Token&, Tokens&) -> std::unique_ptr<AST::Variable>;
  [[nodiscard]] auto parse_call(Token&, Tokens&) -> std::unique_ptr<AST::Call>;

  [[nodiscard]] auto parse_func(Token&, Tokens&) -> std::unique_ptr<AST::Func>;

  [[nodiscard]] auto parse_type(Token&, Tokens&) -> std::unique_ptr<AST::Type>;
  [[nodiscard]] auto parse_attribute(Token&, Tokens&) -> AST::Attribute;

  [[nodiscard]] auto parse_assignment(Token&, Tokens&) -> std::unique_ptr<AST::Assignment>;
  [[nodiscard]] auto parse_return(Token&, Tokens&) -> std::unique_ptr<AST::Return>;
  [[nodiscard]] auto parse_declaration(Token&, Tokens&) -> std::unique_ptr<AST::Declaration>;
  [[nodiscard]] auto parse_print(Token&, Tokens&) -> std::unique_ptr<AST::Print>;

  [[nodiscard]] auto parse_if(Token&, Tokens&) -> std::unique_ptr<AST::If>;
  [[nodiscard]] auto parse_while(Token&, Tokens&) -> std::unique_ptr<AST::While>;
};
}  // namespace kuso