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

  Token _lookahead;

  auto match(Token::Type, Tokens&) -> Token;
  auto consume(Tokens&) -> Token;

  [[noreturn]] static void syntax_error(const Token&, const Token&);

  [[nodiscard]] auto parse_statement(Tokens&) -> AST::Statement;
  [[nodiscard]] auto parse_exit(Tokens&) -> std::unique_ptr<AST::Exit>;
  [[nodiscard]] auto parse_expression(Token&, Tokens&) -> std::unique_ptr<AST::Expression>;
  [[nodiscard]] auto parse_assignment(Token&, std::optional<std::unique_ptr<AST::Declaration>>, Tokens&)
      -> std::unique_ptr<AST::Assignment>;
  [[nodiscard]] auto parse_push(Token&, Tokens&) -> std::unique_ptr<AST::Push>;
  [[nodiscard]] auto parse_return(Tokens&) -> std::unique_ptr<AST::Return>;
  [[nodiscard]] auto parse_declaration(Token&, Tokens&) -> std::unique_ptr<AST::Declaration>;
  [[nodiscard]] auto parse_binary_expression(Token&, Tokens&) -> std::unique_ptr<AST::BinaryExpression>;
};
}  // namespace kuso