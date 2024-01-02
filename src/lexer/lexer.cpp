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
  // std::cout << "tokenize\n";
  std::vector<Token> tokens;
  _source = str;
  _iter = _source.begin();
  _line = 1;
  _col = 1;
  Token token{Token::Type::ASTERISK};

  while (token.type != Token::Type::END_OF_FILE && token.type != Token::Type::INVALID) {
    token = parse_token();
    tokens.push_back(token);
  }

  return tokens;
}

auto Lexer::tokenize(const std::filesystem::path& path) -> std::vector<Token> {
  // std::cout << "tokenize\n";
  belt::File sourcefile(path, std::ios::in);
  if (!sourcefile.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }

  auto source = sourcefile.read();
  return tokenize(source);
}

auto Lexer::by_token(const std::filesystem::path& path) -> belt::Generator<Token> {
  // std::cout << "by_token\n";
  belt::File sourcefile(path, std::ios::in);
  if (!sourcefile.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }

  auto source = sourcefile.read();
  return by_token(source);
}

auto Lexer::by_token(std::string src) -> belt::Generator<Token> {
  // std::cout << "by_token\n";
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
auto Lexer::parse_token() noexcept -> Token {
  // std::cout << "parse_token\n";
  if (_iter >= _source.end()) {
    return Token(Token::Type::END_OF_FILE, _line, _col);
  }

  while (*_iter == '/' || std::iswspace(*_iter)) {
    if (std::iswspace(*_iter)) {
      next();
      continue;
    }
    if (_iter + 1 < _source.end()) {
      if (*(_iter + 1) == '/') {
        skip_comments(false);
      } else if (*(_iter + 1) == '*') {
        skip_comments(true);
      } else {
        break;
      }
    } else {
      break;
    }
  }

  if (_iter >= _source.end()) {
    return Token(Token::Type::END_OF_FILE, _line, _col);
  }

  switch (*_iter) {
    case ';':
      next();
      return Token(Token::Type::SEMI_COLON, _line, _col);
      break;
    case ',':
      next();
      return Token(Token::Type::COMMA, _line, _col);
      break;
    case '(':
      next();
      return Token(Token::Type::OPEN_PAREN, _line, _col);
      break;
    case ')':
      next();
      return Token(Token::Type::CLOSE_PAREN, _line, _col);
      break;
    case '{':
      next();
      return Token(Token::Type::OPEN_BRACE, _line, _col);
      break;
    case '}':
      next();
      return Token(Token::Type::CLOSE_BRACE, _line, _col);
      break;
    case '[':
      next();
      return Token(Token::Type::OPEN_BRACKET, _line, _col);
      break;
    case ']':
      next();
      return Token(Token::Type::CLOSE_BRACKET, _line, _col);
      break;
    case ':':
      next();
      return Token(Token::Type::COLON, _line, _col);
      break;
    case '.':
      next();
      return Token(Token::Type::DOT, _line, _col);
      break;
    case '#':
      next();
      return Token(Token::Type::HASH, _line, _col);
      break;
    case '@':
      next();
      return Token(Token::Type::AT, _line, _col);
      break;
    case '&':
      next();
      return Token(Token::Type::AMPERSAND, _line, _col);
      break;
    case '*':
      next();
      return Token(Token::Type::ASTERISK, _line, _col);
      break;
    case '+':
      next();
      return Token(Token::Type::PLUS, _line, _col);
      break;
    case '-':
      next();
      if (*_iter == '>') {
        next();
        return Token(Token::Type::ARROW, _line, _col);
      }
      return Token(Token::Type::MINUS, _line, _col);
      break;
    case '%':
      next();
      return Token(Token::Type::PERCENT, _line, _col);
      break;
    case '^':
      next();
      return Token(Token::Type::CARET, _line, _col);
      break;
    case '~':
      next();
      return Token(Token::Type::TILDE, _line, _col);
      break;
    case '!':
      next();
      return replace_not_equal();
      break;
    case '?':
      next();
      return Token(Token::Type::QUESTION, _line, _col);
      break;
    case '<':
      next();
      return replace_gt_lt(false);
      break;
    case '>':
      next();
      return replace_gt_lt(true);
      break;
    case '=':
      next();
      return replace_bool_equal();
      break;
    case '/':
      next();
      return Token(Token::Type::SLASH, _line, _col);
      break;
    case '|':
      next();
      return Token(Token::Type::PIPE, _line, _col);
      break;
    case '"':
      next();
      return parse_string();
      break;
    case '\'':
      next();
      return Token(Token::Type::SINGLE_QUOTE, _line, _col);
      break;
    case '\\':
      next();
      return Token(Token::Type::BACKSLASH, _line, _col);
      break;
    case '`':
      next();
      return Token(Token::Type::BACKTICK, _line, _col);
      break;
    case '$':
      next();
      return Token(Token::Type::DOLLAR, _line, _col);
      break;
    case '_':
      next();
      return Token(Token::Type::UNDERSCORE, _line, _col);
      break;
    default: {
      if (std::isalpha(*_iter)) {
        auto token = parse_identifier();

        if (token.type == Token::Type::ASM) {
          return parse_asm();
        }
        return token;
      }
      if (std::isdigit(*_iter)) {
        return parse_number();
      }

      return Token(Token::Type::INVALID, _line, _col, fmt::format("{}", *_iter));
    }
  }
}

/**
 * @brief Skips comments
 * 
 * @return char last character after the comment
 */
void Lexer::skip_comments(bool multiline) {
  // std::cout << "skip_comments\n";
  if (multiline) {
    while (_iter < _source.end() - 1) {
      next();
      if (*_iter == '*' && *(_iter + 1) == '/') {
        next();
        next();
        return;
      }
    }
    return;
  }

  while (_iter < _source.end() && *_iter != '\n') {
    next();
  }
}

/**
 * @brief Parses an identifier, returning it as a keyword if it is one
 * 
 * @param *_iter first character of the identifier
 * @return Token resulting token
 */
auto Lexer::parse_identifier() noexcept -> Token {
  // std::cout << "parse_identifier\n";
  std::string value;
  value += *_iter;

  next();
  while ((_iter < _source.end()) && std::isalnum(*_iter)) {
    value += *_iter;
    next();
  }

  auto type = replace_keyword_type(value);
  if (type == Token::Type::IDENTIFIER) {
    return Token(type, _line, _col, value);
  }

  return Token(type, _line, _col);
}

/**
 * @brief Parses a number
 * 
 * @param *_iter first character of the number
 * @return Token resulting token
 */
auto Lexer::parse_number() noexcept -> Token {
  // std::cout << "parse_number\n";
  std::string value;
  value += *_iter;

  next();
  while ((_iter < _source.end()) && std::isdigit(*_iter)) {
    value += *_iter;
    next();
  }

  return Token(Token::Type::NUMBER, _line, _col, value);
}

auto Lexer::parse_asm() noexcept -> Token {
  // std::cout << "parse_asm\n";
  // TODO(tom): adding character by character to a string is slow
  std::string value;
  while (std::iswspace(*_iter)) {
    if (!next()) {
      return Token(Token::Type::INVALID, _line, _col, value);
    }
  }

  if (*_iter != '{') {
    return Token(Token::Type::INVALID, _line, _col, value);
  }

  next();
  while ((_iter < _source.end()) && *_iter != '}') {
    value += *_iter;
    next();
  }

  if (_iter == _source.end()) {
    return Token(Token::Type::INVALID, _line, _col, value);
  }

  next();
  return Token(Token::Type::ASM, _line, _col, value);
}

/**
 * @brief Parses a string
 * 
 * @return Token resulting token
 */
auto Lexer::parse_string() noexcept -> Token {
  // std::cout << "parse_string\n";
  std::string value = "\"";

  next();
  while ((_iter < _source.end()) && *_iter != '"') {
    value += *_iter;
    next();
  }

  if (_iter == _source.end()) {
    return Token(Token::Type::INVALID, _line, _col, value);
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
  // std::cout << "replace_bool_equal\n";
  if (_iter >= _source.end()) return Token(Token::Type::EQUAL, _line, _col);

  if (*_iter == '=') {
    next();
    return Token(Token::Type::BOOL_EQUAL, _line, _col);
  }

  return Token(Token::Type::EQUAL, _line, _col);
}

/**
 * @brief Replaces a greater than or less than operator with a greater than or less than equal operator, if it is one
 * 
 * @param greater whether the operator is a greater than or less than operator
 * @return Token resulting token
 */
auto Lexer::replace_gt_lt(bool greater) -> Token {
  // std::cout << "replace_gt_lt\n";
  if (_iter >= _source.end())
    return greater ? Token(Token::Type::GREATER_THAN, _line, _col)
                   : Token(Token::Type::LESS_THAN, _line, _col);

  if (*_iter == '=') {
    next();
    if (greater) {
      return Token(Token::Type::GREATER_THAN_EQUAL, _line, _col);
    }
    return Token(Token::Type::LESS_THAN_EQUAL, _line, _col);
  }

  return greater ? Token(Token::Type::GREATER_THAN, _line, _col) : Token(Token::Type::LESS_THAN, _line, _col);
}

/**
 * @brief Replaces a not equal operator with a not equal operator, if it is one
 * 
 * @return Token resulting token
 */
auto Lexer::replace_not_equal() -> Token {
  // std::cout << "replace_not_equal\n";
  if (_iter >= _source.end()) return Token(Token::Type::EXCLAMATION, _line, _col);

  if (*_iter == '=') {
    next();
    return Token(Token::Type::NOT_EQUAL, _line, _col);
  }

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
  // std::cout << "is_keyword\n";
  return keywords().find(value) != keywords().end();
}

/**
 * @brief Replaces a keyword with its corresponding token type
 * 
 * @param value keyword to replace
 * @return Token::Type resulting token type
 */
auto Lexer::replace_keyword_type(const std::string& value) -> Token::Type {
  // std::cout << "replace_keyword_type\n";
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
  // std::cout << "keywords\n";
  static const std::map<std::string, Token::Type> KEYWORDS{
      {"if", Token::Type::IF},       {"else", Token::Type::ELSE},     {"for", Token::Type::FOR},
      {"while", Token::Type::WHILE}, {"return", Token::Type::RETURN}, {"exit", Token::Type::EXIT},
      {"type", Token::Type::TYPE},   {"func", Token::Type::FUNC},     {"main", Token::Type::MAIN},
      {"asm", Token::Type::ASM}};
  return KEYWORDS;
}
}  // namespace kuso