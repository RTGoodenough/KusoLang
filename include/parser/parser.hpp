#pragma once

#include <initializer_list>
#include <optional>

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"

namespace kuso {
class Parser {
  NON_DEFAULT_CONSTRUCTIBLE(Parser)
  NON_COPYABLE(Parser)

  DEFAULT_DESTRUCTIBLE(Parser)
  DEFAULT_MOVABLE(Parser)

  using Tokens = belt::Generator<Token>;

 public:
  explicit Parser(const std::filesystem::path& source) : _lexer(source) {}

  [[nodiscard]] auto parse() -> AST;

 private:
  Lexer _lexer;

  Token _lookahead;

  auto match(std::initializer_list<Token::Type>, Token&, Tokens&) -> bool;
  auto consume(Tokens&) -> Token;

  [[noreturn]] static void syntax_error(const Token&, const Token&);

  [[nodiscard]] auto parse_statement(Tokens&) -> AST::Statement;
  [[nodiscard]] auto parse_exit(Token&, Tokens&) -> std::unique_ptr<AST::Exit>;

  [[nodiscard]] auto parse_expression(Token&, Tokens&) -> std::unique_ptr<AST::Expression>;
  [[nodiscard]] auto parse_equality(Token&, Tokens&) -> std::unique_ptr<AST::Equality>;
  [[nodiscard]] auto parse_comparison(Token&, Tokens&) -> std::unique_ptr<AST::Comparison>;
  [[nodiscard]] auto parse_term(Token&, Tokens&) -> std::unique_ptr<AST::Term>;
  [[nodiscard]] auto parse_factor(Token&, Tokens&) -> std::unique_ptr<AST::Factor>;
  [[nodiscard]] auto parse_unary(Token&, Tokens&) -> std::unique_ptr<AST::Unary>;
  [[nodiscard]] auto parse_primary(Token&, Tokens&) -> std::unique_ptr<AST::Primary>;

  [[nodiscard]] auto parse_assignment(Token&, Tokens&) -> std::unique_ptr<AST::Assignment>;
  [[nodiscard]] auto parse_push(Token&, Tokens&) -> std::unique_ptr<AST::Push>;
  [[nodiscard]] auto parse_return(Token&, Tokens&) -> std::unique_ptr<AST::Return>;
  [[nodiscard]] auto parse_declaration(Token&, Tokens&) -> std::unique_ptr<AST::Declaration>;
  [[nodiscard]] auto parse_print(Token&, Tokens&) -> std::unique_ptr<AST::Print>;
  [[nodiscard]] auto parse_if(Token&, Tokens&) -> std::unique_ptr<AST::If>;
};
}  // namespace kuso