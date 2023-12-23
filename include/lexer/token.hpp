/**
 * @file token.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <belt/class_macros.hpp>
#include <string>
#include <unordered_map>

namespace kuso {
/**
 * @brief Token class
 * 
 */
struct Token {
  /**
   * @brief Token types
   * 
   */
  enum class Type {
    INVALID,
    IDENTIFIER,
    KEYWORD,
    RETURN,
    TYPE,
    PRINT,
    EXIT,
    IF,
    ELSE,
    FOR,
    WHILE,
    OPERATOR,
    NUMBER,
    STRING,
    END_OF_FILE,
    SEMI_COLON,
    COMMA,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    COLON,
    DOT,
    ARROW,
    HASH,
    AT,
    AMPERSAND,
    ASTERISK,
    PLUS,
    MINUS,
    SLASH,
    PERCENT,
    CARET,
    TILDE,
    EXCLAMATION,
    QUESTION,
    LESS_THAN,
    GREATER_THAN,
    EQUAL,
    BOOL_EQUAL,
    NOT_EQUAL,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    PIPE,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    BACKSLASH,
    BACKTICK,
    DOLLAR,
    UNDERSCORE,
    COUNT,
  };

  Type        type{Type::END_OF_FILE};
  int         line{0};
  int         column{0};
  std::string value;

  explicit Token(Type type, int lineNum = 0, int colNum = 0, std::string value = "") noexcept
      : type(type), line(lineNum), column(colNum), value(std::move(value)) {}
  DEFAULT_CONSTRUCTIBLE(Token)
  DEFAULT_COPYABLE(Token)
  DEFAULT_DESTRUCTIBLE(Token)
  DEFAULT_MOVABLE(Token)
};

/**
 * @brief Convert token to string
 * 
 * @param token Token to convert
 * @return std::string String representation of token
 */
inline auto to_string(const Token& token) -> std::string {
  static const std::unordered_map<Token::Type, std::string> TYPE_MAP{
      {Token::Type::IDENTIFIER, "IDENTIFIER"},
      {Token::Type::KEYWORD, "KEYWORD"},
      {Token::Type::RETURN, "RETURN"},
      {Token::Type::TYPE, "TYPE"},
      {Token::Type::IF, "IF"},
      {Token::Type::ELSE, "ELSE"},
      {Token::Type::EXIT, "EXIT"},
      {Token::Type::FOR, "FOR"},
      {Token::Type::WHILE, "WHILE"},
      {Token::Type::OPERATOR, "OPERATOR"},
      {Token::Type::NUMBER, "NUMBER"},
      {Token::Type::STRING, "STRING"},
      {Token::Type::END_OF_FILE, "END_OF_FILE"},
      {Token::Type::SEMI_COLON, "SEMI_COLON"},
      {Token::Type::COMMA, "COMMA"},
      {Token::Type::OPEN_PAREN, "OPEN_PAREN"},
      {Token::Type::CLOSE_PAREN, "CLOSE_PAREN"},
      {Token::Type::OPEN_BRACE, "OPEN_BRACE"},
      {Token::Type::CLOSE_BRACE, "CLOSE_BRACE"},
      {Token::Type::OPEN_BRACKET, "OPEN_BRACKET"},
      {Token::Type::CLOSE_BRACKET, "CLOSE_BRACKET"},
      {Token::Type::COLON, "COLON"},
      {Token::Type::DOT, "DOT"},
      {Token::Type::ARROW, "ARROW"},
      {Token::Type::HASH, "HASH"},
      {Token::Type::AT, "AT"},
      {Token::Type::AMPERSAND, "AMPERSAND"},
      {Token::Type::ASTERISK, "ASTERISK"},
      {Token::Type::PLUS, "PLUS"},
      {Token::Type::MINUS, "MINUS"},
      {Token::Type::SLASH, "SLASH"},
      {Token::Type::PERCENT, "PERCENT"},
      {Token::Type::CARET, "CARET"},
      {Token::Type::TILDE, "TILDE"},
      {Token::Type::EXCLAMATION, "EXCLAMATION"},
      {Token::Type::QUESTION, "QUESTION"},
      {Token::Type::LESS_THAN, "LESS_THAN"},
      {Token::Type::GREATER_THAN, "GREATER_THAN"},
      {Token::Type::EQUAL, "EQUAL"},
      {Token::Type::PIPE, "PIPE"},
      {Token::Type::DOUBLE_QUOTE, "DOUBLE_QUOTE"},
      {Token::Type::SINGLE_QUOTE, "SINGLE_QUOTE"},
      {Token::Type::BACKSLASH, "BACKSLASH"},
      {Token::Type::BACKTICK, "BACKTICK"}};
  auto type = TYPE_MAP.find(token.type);

  if (type == TYPE_MAP.end()) {
    return "Token { type: UNKNOWN value: " + token.value + "}";
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  return "Token { type: " + type->second + " value: " + token.value + "}";
}
}  // namespace kuso