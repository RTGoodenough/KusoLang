

#include <iostream>
#include <memory>
#include <stdexcept>

#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"

namespace kuso {

void Parser::syntax_error(const Token& token, const Token& expected) {
  std::cerr << "Syntax error: \nExpected:\n"
            << to_string(expected) << "\nFound:\n"
            << to_string(token) << std::endl;

  std::exit(1);
}

auto Parser::parse() -> AST {
  AST  ast;
  auto tokens = _lexer.by_token();
  consume(tokens);

  while (tokens.has_next()) {
    if (_lookahead.type == Token::Type::END_OF_FILE) {
      break;
    }
    ast.add_statement(parse_statement(tokens));
  }

  return ast;
}

auto Parser::parse_statement(Tokens& tokens) -> AST::Statement {
  AST::Statement statement{nullptr};
  auto           token = consume(tokens);

  switch (token.type) {
    case Token::Type::IDENTIFIER:
      statement.statement = parse_assignment(token, tokens);
      break;
    case Token::Type::RETURN:
      statement.statement = parse_return(tokens);
      break;
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
  }

  match(Token::Type::SEMI_COLON, tokens);
  return statement;
}

auto Parser::parse_expression(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Expression> {
  auto expression = std::make_unique<AST::Expression>();

  switch (_lookahead.type) {
    case Token::Type::PLUS:
    case Token::Type::MINUS:
    case Token::Type::ASTERISK:
    case Token::Type::SLASH:
    case Token::Type::PERCENT:
    case Token::Type::CARET:
      expression->value = parse_binary_expression(token, tokens);
      return expression;
    case Token::Type::ARROW:
      throw std::runtime_error("Chained push not implemented");
      break;
    default:
      break;
  }

  printf("lookahead: %s\n", to_string(_lookahead).c_str());

  expression->value = std::make_unique<AST::Terminal>(token);
  return expression;
}

auto Parser::parse_assignment(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Assignment> {
  auto assignment = std::make_unique<AST::Assignment>();
  if (_lookahead.type == Token::Type::COLON)
    assignment->dest = parse_declaration(dest, tokens);
  else
    assignment->dest = std::make_unique<AST::Terminal>(dest);
  match(Token::Type::EQUAL, tokens);
  dest = consume(tokens);
  assignment->value = parse_expression(dest, tokens);
  return assignment;
}

auto Parser::parse_push(Token&, Tokens&) -> std::unique_ptr<AST::Push> {
  throw std::runtime_error("Pushes Not implemented");
}

auto Parser::parse_declaration(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Declaration> {
  std::unique_ptr<AST::Declaration> declaration = std::make_unique<AST::Declaration>();
  declaration->name = std::make_unique<AST::Terminal>(dest);
  match(Token::Type::COLON, tokens);
  dest = match(Token::Type::IDENTIFIER, tokens);
  declaration->type = std::make_unique<AST::Terminal>(dest);
  return declaration;
}

auto Parser::parse_binary_expression(Token& token, Tokens& tokens) -> std::unique_ptr<AST::BinaryExpression> {
  auto expression = std::make_unique<AST::BinaryExpression>();

  expression->left = std::make_unique<AST::Expression>(std::make_unique<AST::Terminal>(token));
  switch (_lookahead.type) {
    case Token::Type::PLUS:
      expression->op = AST::BinaryOp::PLUS;
      break;
    case Token::Type::MINUS:
      expression->op = AST::BinaryOp::MINUS;
      break;
    case Token::Type::ASTERISK:
      expression->op = AST::BinaryOp::ASTERISK;
      break;
    case Token::Type::SLASH:
      expression->op = AST::BinaryOp::SLASH;
      break;
    case Token::Type::PERCENT:
      expression->op = AST::BinaryOp::PERCENT;
      break;
    case Token::Type::CARET:
      expression->op = AST::BinaryOp::CARET;
      break;
    default:
      syntax_error(_lookahead, Token(Token::Type::PLUS));
  }
  consume(tokens);
  token = consume(tokens);
  expression->right = parse_expression(token, tokens);
  return expression;
}

auto Parser::parse_return(Tokens& tokens) -> std::unique_ptr<AST::Return> {
  auto returnStatement = std::make_unique<AST::Return>();
  auto token = consume(tokens);
  returnStatement->value = parse_expression(token, tokens);
  return returnStatement;
}

auto Parser::match(Token::Type type, Tokens& tokens) -> Token {
  if (_lookahead.type == type) {
    return consume(tokens);
  }

  syntax_error(_lookahead, Token(type));
}

auto Parser::consume(Tokens& tokens) -> Token {
  Token temp = _lookahead;
  _lookahead = tokens.next();
  return temp;
}
}  // namespace kuso