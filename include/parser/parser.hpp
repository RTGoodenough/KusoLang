#pragma once

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

  static void syntax_error(const Token&, const Token&);

  void parse_statement(Tokens&);
  void parse_expression(Token, Tokens&);
};
}  // namespace kuso