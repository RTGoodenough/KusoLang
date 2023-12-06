

#include <cctype>
#include <iostream>

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

namespace kuso {

auto Lexer::by_token() -> belt::Generator<Token> {
  while (!_source.eof()) {
    co_yield parse_token();
  }

  co_return Token(Token::Type::END_OF_FILE);
}

auto Lexer::parse_token() -> Token {
  if (_source.eof()) {
    return Token(Token::Type::END_OF_FILE);
  }

  char chr = _source.next_char();
  while (std::isspace(chr)) {
    chr = _source.next_char();
  }
  if (chr == '/') {
    chr = skip_comments();
  }
  while (std::isblank(chr)) {
    chr = _source.next_char();
  }

  if (_source.eof()) {
    return Token(Token::Type::END_OF_FILE);
  }

  switch (chr) {
    case ';':
      return Token(Token::Type::SEMI_COLON);
      break;
    case ',':
      return Token(Token::Type::COMMA);
      break;
    case '(':
      return Token(Token::Type::OPEN_PAREN);
      break;
    case ')':
      return Token(Token::Type::CLOSE_PAREN);
      break;
    case '{':
      return Token(Token::Type::OPEN_BRACE);
      break;
    case '}':
      return Token(Token::Type::CLOSE_BRACE);
      break;
    case '[':
      return Token(Token::Type::OPEN_BRACKET);
      break;
    case ']':
      return Token(Token::Type::CLOSE_BRACKET);
      break;
    case ':':
      return Token(Token::Type::COLON);
      break;
    case '.':
      return Token(Token::Type::DOT);
      break;
    case '#':
      return Token(Token::Type::HASH);
      break;
    case '@':
      return Token(Token::Type::AT);
      break;
    case '&':
      return Token(Token::Type::AMPERSAND);
      break;
    case '*':
      return Token(Token::Type::ASTERISK);
      break;
    case '+':
      return Token(Token::Type::PLUS);
      break;
    case '-':
      if (_source.peek_char() == '>') {
        chr = _source.next_char();
        return Token(Token::Type::ARROW);
      } else {
        return Token(Token::Type::MINUS);
      }
      break;
    case '%':
      return Token(Token::Type::PERCENT);
      break;
    case '^':
      return Token(Token::Type::CARET);
      break;
    case '~':
      return Token(Token::Type::TILDE);
      break;
    case '!':
      return Token(Token::Type::EXCLAMATION);
      break;
    case '?':
      return Token(Token::Type::QUESTION);
      break;
    case '<':
      return Token(Token::Type::LESS_THAN);
      break;
    case '>':
      return Token(Token::Type::GREATER_THAN);
      break;
    case '=':
      return Token(Token::Type::EQUAL);
      break;
    case '/':
      return Token(Token::Type::SLASH);
      break;
    case '|':
      return Token(Token::Type::PIPE);
      break;
    case '"':
      return parse_string(chr);
      break;
    case '\'':
      return Token(Token::Type::SINGLE_QUOTE);
      break;
    case '\\':
      return Token(Token::Type::BACKSLASH);
      break;
    case '`':
      return Token(Token::Type::BACKTICK);
      break;
    case '$':
      return Token(Token::Type::DOLLAR);
      break;
    case '_':
      return Token(Token::Type::UNDERSCORE);
      break;
    default: {
      if (std::isalpha(chr)) {
        return parse_identifier(chr);
      }
      if (std::isdigit(chr)) {
        return parse_number(chr);
      }
      throw std::runtime_error("Unexpected character: " + std::to_string(chr) + '\n');
    }
  }
}

auto Lexer::skip_comments() -> char {
  if (_source.peek_char() == '/') {
    while (_source.next_char() != '\n') {
    }
  }
  return _source.next_char();
}

auto Lexer::parse_identifier(char chr) -> Token {
  std::string value;
  value += chr;
  while (std::isalnum(_source.peek_char()) && !_source.eof()) {
    value += _source.next_char();
  }

  if (is_keyword(value)) {
    return Token(Token::Type::KEYWORD, value);
  }

  return Token(Token::Type::IDENTIFIER, value);
}

auto Lexer::parse_number(char chr) -> Token {
  std::string value;
  value += chr;
  while (std::isdigit(_source.peek_char()) && !_source.eof()) {
    value += _source.next_char();
  }

  return Token(Token::Type::NUMBER, value);
}

auto Lexer::parse_string(char chr) -> Token {
  std::string value;
  value += chr;
  while (_source.peek_char() != '"' && !_source.eof()) {
    value += _source.next_char();
  }

  return Token(Token::Type::STRING, value);
}

auto Lexer::is_keyword(const std::string& value) -> bool {
  return keywords().find(value) != keywords().end();
}

auto Lexer::keywords() -> const std::set<std::string>& {
  static const std::set<std::string> KEYWORDS{
      "if",
      "else",
      "for",
      "while",
  };
  return KEYWORDS;
}

}  // namespace kuso