/**
 * @file lexer.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

#include <fmt/format.h>
#include <cwctype>

namespace kuso {

auto Lexer::tokenize(const std::string& str) -> std::vector<Token> {
  std::vector<Token> tokens;
  _source = str;
  _iter = _source.begin();
  _line = 1;
  _col = 1;
  Token token{Token::Type::ASTERISK};

  while (token.type != Token::Type::END_OF_FILE) {
    token = parse_token();
    tokens.push_back(token);
  }

  return tokens;
}

auto Lexer::tokenize(const std::filesystem::path& path) -> std::vector<Token> {
  belt::File sourcefile(path, std::ios::in);
  if (!sourcefile.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }

  auto source = sourcefile.read();
  return tokenize(source);
}

auto Lexer::by_token(const std::filesystem::path& path) -> belt::Generator<Token> {
  belt::File sourcefile(path, std::ios::in);
  if (!sourcefile.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }

  auto source = sourcefile.read();
  return by_token(source);
}

auto Lexer::by_token(std::string src) -> belt::Generator<Token> {
  auto tokens = tokenize(src);

  for (auto& token : tokens) {
    co_yield token;
  }

  while (true) co_yield Token(Token::Type::END_OF_FILE, 0, 0);
}

/**
 * @brief Parses a token from the source file
 * 
 * @return Token 
 */
auto Lexer::parse_token() -> Token {
  if (_iter == _source.end()) {
    return Token(Token::Type::END_OF_FILE, _line, _col);
  }

  ++_iter;
  ++_col;
  while (std::isspace(*_iter)) {
    if (*_iter == '\n') {
      ++_line;
      _col = 1;
    }
    ++_iter;
    ++_col;
  }
  while (*_iter == '/') {
    skip_comments();
  }

  while (std::iswspace(*_iter)) {
    if (*_iter == '\n') {
      ++_line;
      _col = 1;
    }

    ++_iter;
    ++_col;
  }

  if (_iter == _source.end()) {
    return Token(Token::Type::END_OF_FILE, _line, _col);
  }

  switch (*_iter) {
    case ';':
      return Token(Token::Type::SEMI_COLON, _line, _col);
      break;
    case ',':
      return Token(Token::Type::COMMA, _line, _col);
      break;
    case '(':
      return Token(Token::Type::OPEN_PAREN, _line, _col);
      break;
    case ')':
      return Token(Token::Type::CLOSE_PAREN, _line, _col);
      break;
    case '{':
      return Token(Token::Type::OPEN_BRACE, _line, _col);
      break;
    case '}':
      return Token(Token::Type::CLOSE_BRACE, _line, _col);
      break;
    case '[':
      return Token(Token::Type::OPEN_BRACKET, _line, _col);
      break;
    case ']':
      return Token(Token::Type::CLOSE_BRACKET, _line, _col);
      break;
    case ':':
      return Token(Token::Type::COLON, _line, _col);
      break;
    case '.':
      return Token(Token::Type::DOT, _line, _col);
      break;
    case '#':
      return Token(Token::Type::HASH, _line, _col);
      break;
    case '@':
      return Token(Token::Type::AT, _line, _col);
      break;
    case '&':
      return Token(Token::Type::AMPERSAND, _line, _col);
      break;
    case '*':
      return Token(Token::Type::ASTERISK, _line, _col);
      break;
    case '+':
      return Token(Token::Type::PLUS, _line, _col);
      break;
    case '-':
      ++_iter;
      if (*_iter == '>') {
        ++_col;
        return Token(Token::Type::ARROW, _line, _col);
      }
      --_iter;
      return Token(Token::Type::MINUS, _line, _col);
      break;
    case '%':
      return Token(Token::Type::PERCENT, _line, _col);
      break;
    case '^':
      return Token(Token::Type::CARET, _line, _col);
      break;
    case '~':
      return Token(Token::Type::TILDE, _line, _col);
      break;
    case '!':
      return replace_not_equal();
      break;
    case '?':
      return Token(Token::Type::QUESTION, _line, _col);
      break;
    case '<':
      return replace_gt_lt(false);
      break;
    case '>':
      return replace_gt_lt(true);
      break;
    case '=':
      return replace_bool_equal();
      break;
    case '/':
      return Token(Token::Type::SLASH, _line, _col);
      break;
    case '|':
      return Token(Token::Type::PIPE, _line, _col);
      break;
    case '"':
      return parse_string();
      break;
    case '\'':
      return Token(Token::Type::SINGLE_QUOTE, _line, _col);
      break;
    case '\\':
      return Token(Token::Type::BACKSLASH, _line, _col);
      break;
    case '`':
      return Token(Token::Type::BACKTICK, _line, _col);
      break;
    case '$':
      return Token(Token::Type::DOLLAR, _line, _col);
      break;
    case '_':
      return Token(Token::Type::UNDERSCORE, _line, _col);
      break;
    default: {
      if (std::isalpha(*_iter)) {
        return parse_identifier();
      }
      if (std::isdigit(*_iter)) {
        return parse_number();
      }
      throw std::runtime_error(fmt::format("Invalid character: {}", *_iter));
    }
  }
}

/**
 * @brief Skips comments
 * 
 * @return char last character after the comment
 */
void Lexer::skip_comments() {
  if (*_iter == '/') {
    ++_iter;
    if (*_iter == '/') {
      while (*_iter != '\n' && _iter != _source.end()) {
        ++_iter;
      }
      return;
    }

    if (*_iter == '*') {
      while (*_iter != '*' && *(_iter + 1) != '/' && _iter != _source.end()) {
        ++_iter;
      }
    }
  }
}

/**
 * @brief Parses an identifier, returning it as a keyword if it is one
 * 
 * @param *_iter first character of the identifier
 * @return Token resulting token
 */
auto Lexer::parse_identifier() -> Token {
  std::string value;
  value += *_iter;

  ++_iter;
  while (std::isalnum(*_iter) && !(_iter == _source.end())) {
    value += *_iter;
    ++_col;
    ++_iter;
  }

  auto type = replace_keyword_type(value);
  if (type == Token::Type::IDENTIFIER) {
    return Token(type, _line, _col, value);
  }

  --_iter;
  return Token(type, _line, _col);
}

/**
 * @brief Parses a number
 * 
 * @param *_iter first character of the number
 * @return Token resulting token
 */
auto Lexer::parse_number() -> Token {
  std::string value;
  value += *_iter;

  ++_iter;
  while (std::isdigit(*_iter) && !(_iter == _source.end())) {
    value += *_iter;
    ++_col;
    ++_iter;
  }

  --_iter;
  return Token(Token::Type::NUMBER, _line, _col, value);
}

/**
 * @brief Parses a string
 * 
 * @return Token resulting token
 */
auto Lexer::parse_string() -> Token {
  std::string value = "\"";

  ++_iter;
  while (*_iter != '"' && !(_iter == _source.end())) {
    value += *_iter;
    ++_col;
    ++_iter;
  }

  if (_iter == _source.end()) {
    throw std::runtime_error("Unterminated string");
  }

  value += '"';
  return Token(Token::Type::STRING, _line, _col, value);
}

/**
 * @brief Replaces an assignment operator with a boolean equal operator, if it is one
 * 
 * @return Token resulting token
 */
auto Lexer::replace_bool_equal() -> Token {
  ++_iter;
  if (*_iter == '=') {
    ++_col;
    return Token(Token::Type::BOOL_EQUAL, _line, _col);
  }

  --_iter;
  return Token(Token::Type::EQUAL, _line, _col);
}

/**
 * @brief Replaces a greater than or less than operator with a greater than or less than equal operator, if it is one
 * 
 * @param greater whether the operator is a greater than or less than operator
 * @return Token resulting token
 */
auto Lexer::replace_gt_lt(bool greater) -> Token {
  ++_iter;
  if (*_iter == '=') {
    ++_col;
    if (greater) {
      return Token(Token::Type::GREATER_THAN_EQUAL, _line, _col);
    }
    return Token(Token::Type::LESS_THAN_EQUAL, _line, _col);
  }

  --_iter;
  if (greater) {
    return Token(Token::Type::GREATER_THAN, _line, _col);
  }
  return Token(Token::Type::LESS_THAN, _line, _col);
}

/**
 * @brief Replaces a not equal operator with a not equal operator, if it is one
 * 
 * @return Token resulting token
 */
auto Lexer::replace_not_equal() -> Token {
  ++_iter;
  if (*_iter == '=') {
    ++_col;
    return Token(Token::Type::NOT_EQUAL, _line, _col);
  }

  --_iter;
  return Token(Token::Type::EXCLAMATION, _line, _col);
}

/**
 * @brief Checks if a string is a keyword
 * 
 * @param value string to check
 * @return true if the string is a keyword
 * @return false if the string is not a keyword
 */
auto Lexer::is_keyword(const std::string& value) -> bool {
  return keywords().find(value) != keywords().end();
}

/**
 * @brief Replaces a keyword with its corresponding token type
 * 
 * @param value keyword to replace
 * @return Token::Type resulting token type
 */
auto Lexer::replace_keyword_type(const std::string& value) -> Token::Type {
  auto iter = keywords().find(value);
  if (iter != keywords().end()) {
    return iter->second;
  }

  return Token::Type::IDENTIFIER;
}

/**
 * @brief Gets the keyword map
 * 
 * @return const std::map<std::string, Token::Type>& keyword map
 */
auto Lexer::keywords() -> const std::map<std::string, Token::Type>& {
  static const std::map<std::string, Token::Type> KEYWORDS{
      {"if", Token::Type::IF},       {"else", Token::Type::ELSE},     {"for", Token::Type::FOR},
      {"while", Token::Type::WHILE}, {"return", Token::Type::RETURN}, {"exit", Token::Type::EXIT},
      {"print", Token::Type::PRINT}, {"type", Token::Type::TYPE},     {"while", Token::Type::WHILE},
  };
  return KEYWORDS;
}
}  // namespace kuso