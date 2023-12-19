

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"

namespace kuso {

void Parser::syntax_error(const Token& token, const Token& expected) {
  std::cerr << "Syntax Error: Line " << std::to_string(token.line) << " Column "
            << std::to_string(token.column) << "\nExpected: " << to_string(expected)
            << "\nFound: " << to_string(token) << "\n";

  std::exit(1);
}

auto Parser::parse() -> AST {
  std::cout << "parse\n";
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
  std::cout << "parse_statement\n";
  AST::Statement statement{nullptr};
  auto           token = consume(tokens);

  std::optional<std::unique_ptr<AST::Declaration>> decl;

  switch (token.type) {
    case Token::Type::IDENTIFIER:
      if (_lookahead.type == Token::Type::COLON) {
        statement.statement = parse_declaration(token, tokens);
        break;
      }
      if (_lookahead.type == Token::Type::EQUAL || _lookahead.type == Token::Type::DOT) {
        statement.statement = parse_assignment(token, tokens);
        break;
      }
      break;
    case Token::Type::TYPE:
      statement.statement = parse_type(token, tokens);
      break;
    case Token::Type::RETURN:
      statement.statement = parse_return(token, tokens);
      break;
    case Token::Type::EXIT:
      statement.statement = parse_exit(token, tokens);
      break;
    case Token::Type::PRINT:
      statement.statement = parse_print(token, tokens);
      break;
    case Token::Type::IF:
      statement.statement = parse_if(token, tokens);
      break;
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
  }

  match({Token::Type::SEMI_COLON}, token, tokens);
  return statement;
}

auto Parser::parse_if(Token& token, Tokens& tokens) -> std::unique_ptr<AST::If> {
  std::cout << "parse_if\n";
  // TODO(rolland): add else to if statements
  auto ifStatement = std::make_unique<AST::If>();

  match({Token::Type::OPEN_PAREN}, token, tokens);
  ifStatement->condition = parse_expression(token, tokens);

  match({Token::Type::CLOSE_PAREN}, token, tokens);
  match({Token::Type::OPEN_BRACE}, token, tokens);

  while (token.type != Token::Type::CLOSE_BRACE) {
    ifStatement->body.push_back(parse_statement(tokens));
    token = consume(tokens);
  }

  return ifStatement;
}

auto Parser::parse_type(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Type> {
  std::cout << "parse_type\n";
  auto type = std::make_unique<AST::Type>();

  match({Token::Type::IDENTIFIER}, token, tokens);
  type->name = token.value;

  match({Token::Type::OPEN_BRACE}, token, tokens);

  while (!try_match({Token::Type::CLOSE_BRACE}, token, tokens)) {
    type->attributes.push_back(parse_attribute(token, tokens));
    match({Token::Type::SEMI_COLON}, token, tokens);
  }

  return type;
}

auto Parser::parse_attribute(Token& token, Tokens& tokens) -> AST::Attribute {
  std::cout << "parse_attribute\n";
  auto attribute = AST::Attribute{};

  match({Token::Type::IDENTIFIER}, token, tokens);
  attribute.name = token.value;

  match({Token::Type::COLON}, token, tokens);
  match({Token::Type::IDENTIFIER}, token, tokens);
  attribute.type = token.value;

  return attribute;
}

auto Parser::parse_expression(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Expression> {
  std::cout << "parse_expression\n";
  return std::make_unique<AST::Expression>(parse_equality(token, tokens));
}

auto Parser::parse_equality(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Equality> {
  std::cout << "parse_equality\n";
  auto equality = std::make_unique<AST::Equality>();
  equality->left = parse_comparison(token, tokens);

  while (try_match({Token::Type::BOOL_EQUAL, Token::Type::NOT_EQUAL}, token, tokens)) {
    equality->equal = token.type == Token::Type::BOOL_EQUAL;
    equality->right = parse_equality(token, tokens);
  }

  return equality;
}

auto Parser::parse_comparison(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Comparison> {
  std::cout << "parse_comparison\n";
  auto comparison = std::make_unique<AST::Comparison>();
  comparison->left = parse_term(token, tokens);

  while (try_match({Token::Type::LESS_THAN, Token::Type::GREATER_THAN, Token::Type::LESS_THAN_EQUAL,
                    Token::Type::GREATER_THAN_EQUAL},
                   token, tokens)) {
    switch (token.type) {
      case Token::Type::LESS_THAN:
        comparison->op = AST::BinaryOp::LT;
        break;
      case Token::Type::GREATER_THAN:
        comparison->op = AST::BinaryOp::GT;
        break;
      case Token::Type::LESS_THAN_EQUAL:
        comparison->op = AST::BinaryOp::LTE;
        break;
      case Token::Type::GREATER_THAN_EQUAL:
        comparison->op = AST::BinaryOp::GTE;
        break;
      default:
        break;
    }
    comparison->right = parse_comparison(token, tokens);
  }

  return comparison;
}

auto Parser::parse_term(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Term> {
  std::cout << "parse_term\n";
  auto term = std::make_unique<AST::Term>();
  term->left = parse_factor(token, tokens);

  while (try_match({Token::Type::PLUS, Token::Type::MINUS}, token, tokens)) {
    switch (token.type) {
      case Token::Type::PLUS:
        term->op = AST::BinaryOp::ADD;
        break;
      case Token::Type::MINUS:
        term->op = AST::BinaryOp::SUB;
        break;
      default:
        break;
    }
    term->right = parse_term(token, tokens);
  }

  return term;
}

auto Parser::parse_factor(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Factor> {
  std::cout << "parse_factor\n";
  auto factor = std::make_unique<AST::Factor>();
  factor->left = parse_unary(token, tokens);

  while (try_match({Token::Type::ASTERISK, Token::Type::SLASH, Token::Type::PERCENT}, token, tokens)) {
    switch (token.type) {
      case Token::Type::ASTERISK:
        factor->op = AST::BinaryOp::MUL;
        break;
      case Token::Type::SLASH:
        factor->op = AST::BinaryOp::DIV;
        break;
      case Token::Type::PERCENT:
        factor->op = AST::BinaryOp::MOD;
        break;
      default:
        break;
    }
    factor->right = parse_factor(token, tokens);
  }

  return factor;
}

auto Parser::parse_unary(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Unary> {
  std::cout << "parse_unary\n";
  auto unary = std::make_unique<AST::Unary>();

  while (try_match({Token::Type::MINUS, Token::Type::EXCLAMATION}, token, tokens)) {
    switch (token.type) {
      case Token::Type::MINUS:
        unary->op = AST::BinaryOp::SUB;
        break;
      case Token::Type::EXCLAMATION:
        unary->op = AST::BinaryOp::NOT;
        break;
      default:
        break;
    }
    unary->value = parse_unary(token, tokens);
  }
  unary->value = parse_primary(token, tokens);

  return unary;
}

auto Parser::parse_primary(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Primary> {
  std::cout << "parse_primary\n";
  auto primary = std::make_unique<AST::Primary>();

  if (try_match({Token::Type::STRING}, token, tokens)) {
    primary->value = std::make_unique<AST::String>(token.value);
    return primary;
  }

  if (try_match({Token::Type::NUMBER}, token, tokens)) {
    primary->value = std::make_unique<AST::Terminal>(token);
    return primary;
  }

  if (try_match({Token::Type::IDENTIFIER}, token, tokens)) {
    primary->value = parse_variable(token, tokens);
    return primary;
  }

  if (try_match({Token::Type::OPEN_PAREN}, token, tokens)) {
    primary->value = parse_expression(token, tokens);
    match({Token::Type::CLOSE_PAREN}, token, tokens);
    return primary;
  }

  syntax_error(token, Token(Token::Type::IDENTIFIER));
}

auto Parser::parse_assignment(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Assignment> {
  std::cout << "parse_assignment\n";
  auto assignment = std::make_unique<AST::Assignment>();

  assignment->dest = parse_variable(dest, tokens);

  match({Token::Type::EQUAL}, dest, tokens);

  assignment->value = parse_expression(dest, tokens);

  return assignment;
}

auto Parser::parse_variable(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Variable> {
  std::cout << "parse_variable\n";
  auto variable = std::make_unique<AST::Variable>();

  variable->name = token.value;

  if (try_match({Token::Type::DOT}, token, tokens)) {
    match({Token::Type::IDENTIFIER}, token, tokens);
    variable->attribute = token.value;
  }

  return variable;
}

auto Parser::parse_exit(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Exit> {
  std::cout << "parse_exit\n";
  auto exit = std::make_unique<AST::Exit>();
  exit->value = parse_expression(token, tokens);

  return exit;
}

auto Parser::parse_push(Token&, Tokens&) -> std::unique_ptr<AST::Push> {
  std::cout << "parse_push\n";
  throw std::runtime_error("Pushes Not implemented");
}

auto Parser::parse_declaration(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Declaration> {
  std::cout << "parse_declaration\n";
  std::unique_ptr<AST::Declaration> declaration = std::make_unique<AST::Declaration>();

  declaration->name = dest.value;

  match({Token::Type::COLON}, dest, tokens);
  match({Token::Type::IDENTIFIER}, dest, tokens);

  declaration->type = dest.value;

  if (try_match({Token::Type::EQUAL}, dest, tokens)) {
    declaration->value = parse_expression(dest, tokens);
  }

  return declaration;
}

auto Parser::parse_return(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Return> {
  std::cout << "parse_return\n";
  auto returnStatement = std::make_unique<AST::Return>();

  returnStatement->value = parse_expression(token, tokens);

  return returnStatement;
}

auto Parser::parse_print(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Print> {
  std::cout << "parse_print\n";
  auto print = std::make_unique<AST::Print>();

  print->value = parse_expression(token, tokens);

  return print;
}

void Parser::match(std::initializer_list<Token::Type> types, Token& token, Tokens& tokens) {
  for (auto type : types) {
    if (_lookahead.type == type) {
      token = consume(tokens);
      return;
    }
  }

  syntax_error(_lookahead, Token(*types.begin()));
}

auto Parser::try_match(std::initializer_list<Token::Type> types, Token& token, Tokens& tokens) -> bool {
  for (auto type : types) {
    if (_lookahead.type == type) {
      token = consume(tokens);
      return true;
    }
  }

  return false;
}

auto Parser::consume(Tokens& tokens) -> Token {
  Token temp = _lookahead;
  _lookahead = tokens.next();
  return temp;
}
}  // namespace kuso