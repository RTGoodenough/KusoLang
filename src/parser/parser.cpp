

#include <iostream>
#include <memory>
#include <stdexcept>

#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"

namespace kuso {

void Parser::syntax_error(const Token& token, const Token& expected) {
  std::cerr << "Syntax error: expected \n"
            << to_string(expected) << " but got \n"
            << to_string(token) << std::endl;

  std::exit(1);
}

auto Parser::parse() -> AST {
  auto tokens = _lexer.by_token();

  while (tokens.has_next()) {
    _ast.add_statement(parse_statement(tokens));
  }

  return _ast;
}

auto Parser::parse_statement(Tokens& tokens) -> AST::Statement {
  auto token = tokens.next();

  switch (token.type) {
    case Token::Type::IDENTIFIER:
    case Token::Type::NUMBER:
      return AST::Statement{parse_push(token, tokens)};
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
  }

  return AST::Statement{nullptr};
}

auto Parser::parse_expression(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Expression> {
  auto expression = std::make_unique<AST::Expression>();
  auto next = tokens.next();

  switch (next.type) {
    // if its an operator
    case Token::Type::PLUS:
    case Token::Type::MINUS:
    case Token::Type::ASTERISK:
    case Token::Type::SLASH:
    case Token::Type::PERCENT:
    case Token::Type::CARET:
      expression->value = parse_binary_expression(token, next, tokens);
      break;
    case Token::Type::ARROW:
      throw std::runtime_error("Chained push not implemented");
      break;
    default:
      syntax_error(next, Token(Token::Type::PLUS));
  }

  return expression;
}

auto Parser::parse_push(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Push> {
  auto push = std::make_unique<AST::Push>();
  push->value = parse_expression(token, tokens);
  match(Token::Type::ARROW, tokens);
  push->dest = parse_expression(token, tokens);
  return push;
}

auto Parser::parse_declaration(Token&, Tokens&) -> std::unique_ptr<AST::Declaration> {
  throw std::runtime_error("Declarations Not implemented");
}

auto Parser::parse_binary_expression(Token&, Token&, Tokens&) -> std::unique_ptr<AST::BinaryExpression> {
  throw std::runtime_error("Binary Expressions Not implemented");
}

void Parser::match(Token::Type type, Tokens& tokens) {
  if (_curr.type == type) {
    _curr = _lookahead;
    _lookahead = tokens.next();
  } else {
    syntax_error(_lookahead, Token(type));
  }
}

void Parser::consume(Tokens& tokens) {
  _curr = _lookahead;
  _lookahead = tokens.next();
}

}  // namespace kuso