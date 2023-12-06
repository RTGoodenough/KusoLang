#pragma once

#include <belt/class_macros.hpp>
#include <string>
#include <unordered_map>

namespace kuso {
struct Token {
  DEFAULT_CONSTRUCTIBLE(Token)
  DEFAULT_COPYABLE(Token)
  DEFAULT_DESTRUCTIBLE(Token)
  DEFAULT_MOVABLE(Token)

  enum class Type {
    IDENTIFIER,
    KEYWORD,
    RETURN,
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
    PIPE,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    BACKSLASH,
    BACKTICK,
    DOLLAR,
    UNDERSCORE,
  };

  Type        type;
  std::string value;

  explicit Token(Type type, std::string value = "") noexcept : type(type), value(std::move(value)) {}
};

inline auto to_string(const Token& token) -> std::string {
  static const std::unordered_map<Token::Type, std::string> TYPE_MAP{
      {Token::Type::IDENTIFIER, "IDENTIFIER"},
      {Token::Type::KEYWORD, "KEYWORD"},
      {Token::Type::RETURN, "RETURN"},
      {Token::Type::IF, "IF"},
      {Token::Type::ELSE, "ELSE"},
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