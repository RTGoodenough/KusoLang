#pragma once

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
  AST   _ast;

  Token _curr;
  Token _lookahead;

  void match(Token::Type, Tokens&);
  void consume(Tokens&);

  static void syntax_error(const Token&, const Token&);

  [[nodiscard]] auto parse_statement(Tokens&) -> AST::Statement;
  [[nodiscard]] auto parse_expression(Token&, Tokens&) -> std::unique_ptr<AST::Expression>;
  [[nodiscard]] auto parse_push(Token&, Tokens&) -> std::unique_ptr<AST::Push>;
  [[nodiscard]] auto parse_declaration(Token&, Tokens&) -> std::unique_ptr<AST::Declaration>;
  [[nodiscard]] auto parse_binary_expression(Token&, Token&, Tokens&)
      -> std::unique_ptr<AST::BinaryExpression>;
};
}  // namespace kuso