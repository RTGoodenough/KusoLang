/**
 * @file parser.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <fmt/format.h>

#include "lexer/token.hpp"
#include "logging/logging.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"

namespace kuso {

/**
 * @brief Prints a syntax error and exits the program
 * 
 * @param token token found
 * @param expected expected token
 */
void Parser::syntax_error(const Token& token, const Token& expected) {
  throw ParseError(fmt::format("Syntax Error: Line {} Column {}\nExpected: {}\nFound: {}", token.line,
                               token.column, to_string(expected), to_string(token)));
}

/**
 * @brief Constructs an AST from a list of tokens
 * 
 * @return AST 
 */
auto Parser::parse(const std::filesystem::path& sourcepath) -> std::optional<AST> {
  // std::cout << "parse\n";
  AST  ast;
  auto tokens = _lexer.by_token(sourcepath);

  consume(tokens);
  auto token = consume(tokens);

  try {
    while (tokens.has_next()) {
      if (_lookahead.type == Token::Type::END_OF_FILE) {
        break;
      }
      ast.add_statement(parse_statement(token, tokens));
    }
  } catch (const ParseError& e) {
    Logging::error(e.what());
    return std::nullopt;
  }

  return ast;
}

[[nodiscard]] auto token_gen(std::reference_wrapper<const std::vector<Token>> tokens)
    -> belt::Generator<Token> {
  auto token = tokens.get().begin();
  while (token != tokens.get().end()) {
    co_yield Token(*token);
    token++;
  }
  while (true) {
    co_yield Token(Token::Type::END_OF_FILE);
  }
}

auto Parser::parse(const std::vector<Token>& tokens) -> std::optional<AST> {
  // std::cout << "parse\n";
  AST ast;

  auto tokenGen = token_gen(std::cref(tokens));

  consume(tokenGen);
  auto token = consume(tokenGen);

  try {
    while (tokenGen.has_next()) {
      if (_lookahead.type == Token::Type::END_OF_FILE) {
        break;
      }
      ast.add_statement(parse_statement(token, tokenGen));
    }
  } catch (const ParseError& e) {
    Logging::error(e.what());
    return std::nullopt;
  }

  return ast;
}

/**
 * @brief Parses a statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return AST::Statement 
 */
auto Parser::parse_statement(Token& token, Tokens& tokens) -> AST::Statement {
  // std::cout << "parse_statement\n";
  AST::Statement statement{nullptr};

  switch (token.type) {
    case Token::Type::IDENTIFIER:
      if (_lookahead.type == Token::Type::COLON) {
        statement.statement = parse_declaration(token, tokens);
        break;
      }

      if (_lookahead.type == Token::Type::OPEN_PAREN) {
        statement.statement = parse_call(token, tokens);
        break;
      }

      if (_lookahead.type == Token::Type::EQUAL || _lookahead.type == Token::Type::DOT) {
        statement.statement = parse_assignment(token, tokens);
        break;
      }
      syntax_error(token, Token(Token::Type::COLON));
      break;
    case Token::Type::ASM:
      statement.statement = parse_asm(token, tokens);
      break;
    case Token::Type::WHILE:
      statement.statement = parse_while(token, tokens);
      break;
    case Token::Type::TYPE:
      statement.statement = parse_type(token, tokens);
      break;
    case Token::Type::MAIN:
      statement.statement = parse_main(token, tokens);
      break;
    case Token::Type::RETURN:
      statement.statement = parse_return(token, tokens);
      break;
    case Token::Type::EXIT:
      statement.statement = parse_exit(token, tokens);
      break;
    case Token::Type::FUNC:
      statement.statement = parse_func(token, tokens);
      break;
    case Token::Type::IF:
      statement.statement = parse_if(token, tokens);
      break;
    default:
      syntax_error(token, Token(Token::Type::IDENTIFIER));
  }

  match({Token::Type::SEMI_COLON}, token, tokens);
  token = consume(tokens);
  return statement;
}

/**
 * @brief Parses an if statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::If> 
 */
auto Parser::parse_if(Token& token, Tokens& tokens) -> std::unique_ptr<AST::If> {
  // std::cout << "parse_if\n";
  // TODO(rolland): add else to if statements
  auto ifStatement = std::make_unique<AST::If>();

  match({Token::Type::OPEN_PAREN}, token, tokens);
  ifStatement->condition = parse_expression(token, tokens);

  match({Token::Type::CLOSE_PAREN}, token, tokens);
  match({Token::Type::OPEN_BRACE}, token, tokens);
  token = consume(tokens);

  while (token.type != Token::Type::CLOSE_BRACE) {
    ifStatement->body.push_back(parse_statement(token, tokens));
  }

  if (try_match({Token::Type::ELSE}, token, tokens)) {
    match({Token::Type::OPEN_BRACE}, token, tokens);
    token = consume(tokens);

    while (token.type != Token::Type::CLOSE_BRACE) {
      ifStatement->elseBody.push_back(parse_statement(token, tokens));
    }
  }

  return ifStatement;
}

/**
 * @brief Parses a type
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Type> 
 */
auto Parser::parse_type(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Type> {
  // std::cout << "parse_type\n";
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

/**
 * @brief Parses an attribute
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return AST::Attribute 
 */
auto Parser::parse_attribute(Token& token, Tokens& tokens) -> AST::Attribute {
  // std::cout << "parse_attribute\n";
  auto attribute = AST::Attribute{};

  match({Token::Type::IDENTIFIER}, token, tokens);
  attribute.name = token.value;

  match({Token::Type::COLON}, token, tokens);
  match({Token::Type::IDENTIFIER}, token, tokens);
  attribute.type = token.value;

  return attribute;
}

/**
 * @brief Parses an expression
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Expression> 
 */
auto Parser::parse_expression(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Expression> {
  // std::cout << "parse_expression\n";
  return std::make_unique<AST::Expression>(parse_equality(token, tokens));
}

/**
 * @brief Parses an equality
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Equality> 
 */
auto Parser::parse_equality(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Equality> {
  // std::cout << "parse_equality\n";
  auto equality = std::make_unique<AST::Equality>();
  equality->left = parse_comparison(token, tokens);

  while (try_match({Token::Type::BOOL_EQUAL, Token::Type::NOT_EQUAL}, token, tokens)) {
    equality->equal = token.type == Token::Type::BOOL_EQUAL;
    equality->right = parse_equality(token, tokens);
  }

  return equality;
}

/**
 * @brief Parses a comparison
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Comparison> 
 */
auto Parser::parse_comparison(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Comparison> {
  // std::cout << "parse_comparison\n";
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

/**
 * @brief Parses a term
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Term> 
 */
auto Parser::parse_term(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Term> {
  // std::cout << "parse_term\n";
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

/**
 * @brief Parses a factor
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Factor> 
 */
auto Parser::parse_factor(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Factor> {
  // std::cout << "parse_factor\n";
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

/**
 * @brief Parses a unary
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Unary> 
 */
auto Parser::parse_unary(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Unary> {
  // std::cout << "parse_unary\n";
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

/**
 * @brief Parses a primary
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Primary> 
 */
auto Parser::parse_primary(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Primary> {
  // std::cout << "parse_primary\n";
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
    if (_lookahead.type == Token::Type::OPEN_PAREN) {
      primary->value = parse_call(token, tokens);
      return primary;
    }

    primary->value = parse_variable(token, tokens);
    return primary;
  }

  if (try_match({Token::Type::OPEN_PAREN}, token, tokens)) {
    primary->value = parse_expression(token, tokens);
    match({Token::Type::CLOSE_PAREN}, token, tokens);
    return primary;
  }

  syntax_error(_lookahead, Token(Token::Type::IDENTIFIER));
}

/**
 * @brief Parses an assignment
 * 
 * @param dest token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Assignment> 
 */
auto Parser::parse_assignment(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Assignment> {
  // std::cout << "parse_assignment\n";
  auto assignment = std::make_unique<AST::Assignment>();

  assignment->dest = parse_variable(dest, tokens);

  match({Token::Type::EQUAL}, dest, tokens);

  assignment->value = parse_expression(dest, tokens);

  return assignment;
}

/**
 * @brief Parses a variable
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Variable> 
 */
auto Parser::parse_variable(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Variable> {
  // std::cout << "parse_variable\n";
  auto variable = std::make_unique<AST::Variable>();

  variable->name = token.value;

  if (try_match({Token::Type::DOT}, token, tokens)) {
    match({Token::Type::IDENTIFIER}, token, tokens);
    variable->attribute = token.value;
  }

  return variable;
}

/**
 * @brief Parses a while statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::While> 
 */
auto Parser::parse_while(Token& token, Tokens& tokens) -> std::unique_ptr<AST::While> {
  // std::cout << "parse_while\n";
  auto whileStatement = std::make_unique<AST::While>();

  match({Token::Type::OPEN_PAREN}, token, tokens);
  whileStatement->condition = parse_expression(token, tokens);

  match({Token::Type::CLOSE_PAREN}, token, tokens);
  match({Token::Type::OPEN_BRACE}, token, tokens);
  token = consume(tokens);

  while (token.type != Token::Type::CLOSE_BRACE) {
    whileStatement->body.push_back(parse_statement(token, tokens));
  }

  return whileStatement;
}

/**
 * @brief Parses an exit statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Exit> 
 */
auto Parser::parse_exit(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Exit> {
  // std::cout << "parse_exit\n";
  auto exit = std::make_unique<AST::Exit>();

  exit->value = parse_expression(token, tokens);

  return exit;
}

auto Parser::parse_main(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Main> {
  // std::cout << "parse_main\n";
  auto main = std::make_unique<AST::Main>();

  match({Token::Type::OPEN_BRACE}, token, tokens);
  token = consume(tokens);

  while (token.type != Token::Type::CLOSE_BRACE) {
    main->body.push_back(parse_statement(token, tokens));
  }

  return main;
}

auto Parser::parse_func(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Func> {
  // std::cout << "parse_func\n";
  auto func = std::make_unique<AST::Func>();

  match({Token::Type::IDENTIFIER}, token, tokens);
  func->name = token.value;

  match({Token::Type::OPEN_PAREN}, token, tokens);

  while (!try_match({Token::Type::CLOSE_PAREN}, token, tokens)) {
    match({Token::Type::IDENTIFIER}, token, tokens);
    func->args.push_back(parse_declaration(token, tokens));
    if (!try_match({Token::Type::COMMA}, token, tokens)) {
      match({Token::Type::CLOSE_PAREN}, token, tokens);
      break;
    }
  }

  match({Token::Type::ARROW}, token, tokens);
  match({Token::Type::IDENTIFIER}, token, tokens);
  func->returnType = token.value;

  match({Token::Type::OPEN_BRACE}, token, tokens);
  token = consume(tokens);

  while (token.type != Token::Type::CLOSE_BRACE) {
    func->body.push_back(parse_statement(token, tokens));
  }

  return func;
}

/**
 * @brief Parses an inline assembly statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::ASM> 
 */
auto Parser::parse_asm(Token& token, Tokens&) -> std::unique_ptr<AST::ASM> {
  // std::cout << "parse_asm\n";
  auto asmStatement = std::make_unique<AST::ASM>();
  asmStatement->code = token.value;
  return asmStatement;
}

/**
 * @brief Parses a function call
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Call> 
 */
auto Parser::parse_call(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Call> {
  // std::cout << "parse_call\n";
  auto call = std::make_unique<AST::Call>();

  call->name = token.value;

  match({Token::Type::OPEN_PAREN}, token, tokens);

  while (!try_match({Token::Type::CLOSE_PAREN}, token, tokens)) {
    call->args.push_back(parse_expression(token, tokens));
    if (!try_match({Token::Type::COMMA}, token, tokens)) {
      match({Token::Type::CLOSE_PAREN}, token, tokens);
      break;
    }
  }

  return call;
}

/**
 * @brief Parses a declaration
 * 
 * @param dest token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Declaration> 
 */
auto Parser::parse_declaration(Token& dest, Tokens& tokens) -> std::unique_ptr<AST::Declaration> {
  // std::cout << "parse_declaration\n";
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

/**
 * @brief Parses a return statement
 * 
 * @param token token found
 * @param tokens list of tokens
 * @return std::unique_ptr<AST::Return> 
 */
auto Parser::parse_return(Token& token, Tokens& tokens) -> std::unique_ptr<AST::Return> {
  // std::cout << "parse_return\n";
  auto returnStatement = std::make_unique<AST::Return>();

  if (_lookahead.type == Token::Type::SEMI_COLON) {
    return returnStatement;
  }

  returnStatement->value = parse_expression(token, tokens);

  return returnStatement;
}

/**
 * @brief Matches a token, if it doesn't match it prints a syntax error and exits the program
 * 
 * @param types list of types to match
 * @param token token found
 * @param tokens list of tokens
 */
void Parser::match(std::initializer_list<Token::Type> types, Token& token, Tokens& tokens) {
  for (auto type : types) {
    if (_lookahead.type == type) {
      token = consume(tokens);
      return;
    }
  }

  syntax_error(_lookahead, Token(*types.begin()));
}

/**
 * @brief Tries to match a token, if it matches it consumes the token and returns true, otherwise it
 * returns false
 * 
 * @param types list of types to match
 * @param token token found
 * @param tokens list of tokens
 * @return true 
 * @return false 
 */
auto Parser::try_match(std::initializer_list<Token::Type> types, Token& token, Tokens& tokens) -> bool {
  for (auto type : types) {
    if (_lookahead.type == type) {
      token = consume(tokens);
      return true;
    }
  }

  return false;
}

/**
 * @brief Consumes a token from the list of tokens
 * 
 * @param tokens list of tokens
 * @return Token 
 */
auto Parser::consume(Tokens& tokens) -> Token {
  Token temp = _lookahead;
  if (tokens.has_next()) _lookahead = tokens.next();
  return temp;
}
}  // namespace kuso