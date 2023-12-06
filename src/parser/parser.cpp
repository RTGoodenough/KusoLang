

#include <iostream>

#include "lexer/token.hpp"
#include "parser/parser.hpp"

namespace kuso {

void Parser::syntax_error(const Token& token, const Token& expected) {
  std::cerr << "Syntax error: expected \n"
            << to_string(expected) << " but got \n"
            << to_string(token) << std::endl;
}

auto Parser::parse() -> AST {
  auto tokens = _lexer.by_token();

  while (tokens.has_next()) {
    parse_statement(tokens);
  }

  return _ast;
}

void Parser::parse_statement(Tokens& tokens) {
  auto token = tokens.next();

  switch (token.type) {
    case Token::Type::IDENTIFIER:
      parse_expression(token, tokens);
      break;
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
      break;
  }
}

void Parser::parse_expression(Token& token, Tokens& tokens) {
  /*
  id
  number
  id OP expression
  */
  _ast.add_expression();
  switch (token.type) {
    case Token::Type::IDENTIFIER: {
      break;
    }
    case Token::Type::NUMBER: {
      break;
    }
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
      break;
  }
}  // namespace kuso