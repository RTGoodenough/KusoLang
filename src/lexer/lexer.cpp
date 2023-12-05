

#include <iostream>

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

namespace kuso {

auto Lexer::by_token() -> belt::Generator<Token> {
  char chr = 'a';

  while (!_source.eof()) {
    chr = _source.next_char();
    if (std::isspace(chr)) continue;
    if (chr == '/') {
      chr = skip_comments();
    }

    if (_source.eof()) break;

    switch (chr) {
      case ';':
        co_yield Token(Token::Type::SEMI_COLON);
        break;
      case ',':
        co_yield Token(Token::Type::COMMA);
        break;
      case '(':
        co_yield Token(Token::Type::OPEN_PAREN);
        break;
      case ')':
        co_yield Token(Token::Type::CLOSE_PAREN);
        break;
      case '{':
        co_yield Token(Token::Type::OPEN_BRACE);
        break;
      case '}':
        co_yield Token(Token::Type::CLOSE_BRACE);
        break;
      case '[':
        co_yield Token(Token::Type::OPEN_BRACKET);
        break;
      case ']':
        co_yield Token(Token::Type::CLOSE_BRACKET);
        break;
      case ':':
        co_yield Token(Token::Type::COLON);
        break;
      case '.':
        co_yield Token(Token::Type::DOT);
        break;
      case '#':
        co_yield Token(Token::Type::HASH);
        break;
      case '@':
        co_yield Token(Token::Type::AT);
        break;
      case '&':
        co_yield Token(Token::Type::AMPERSAND);
        break;
      case '*':
        co_yield Token(Token::Type::ASTERISK);
        break;
      case '+':
        co_yield Token(Token::Type::PLUS);
        break;
      case '-':
        co_yield Token(Token::Type::MINUS);
        break;
      case '%':
        co_yield Token(Token::Type::PERCENT);
        break;
      case '^':
        co_yield Token(Token::Type::CARET);
        break;
      case '~':
        co_yield Token(Token::Type::TILDE);
        break;
      case '!':
        co_yield Token(Token::Type::EXCLAMATION);
        break;
      case '?':
        co_yield Token(Token::Type::QUESTION);
        break;
      case '<':
        co_yield Token(Token::Type::LESS_THAN);
        break;
      case '>':
        co_yield Token(Token::Type::GREATER_THAN);
        break;
      case '=':
        co_yield Token(Token::Type::EQUAL);
        break;
      case '/':
        co_yield Token(Token::Type::SLASH);
        break;
      case '|':
        co_yield Token(Token::Type::PIPE);
        break;
      case '"':
        co_yield parse_string(chr);
        break;
      case '\'':
        co_yield Token(Token::Type::SINGLE_QUOTE);
        break;
      case '\\':
        co_yield Token(Token::Type::BACKSLASH);
        break;
      case '`':
        co_yield Token(Token::Type::BACKTICK);
        break;
      case '$':
        co_yield Token(Token::Type::DOLLAR);
        break;
      case '_':
        co_yield Token(Token::Type::UNDERSCORE);
        break;
      default: {
        if (std::isalpha(chr)) {
          co_yield parse_identifier(chr);
        } else if (std::isdigit(chr)) {
          co_yield parse_number(chr);
        } else {
          throw std::runtime_error("Unexpected character: " + std::to_string(chr) + '\n');
        }
      }
    }
  }

  co_return Token(Token::Type::END_OF_FILE);
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
  while (std::isalnum(chr = _source.next_char())) {
    if (_source.eof()) throw std::runtime_error("Unexpected end of file while parsing ID\n");
    value += chr;
  }

  return Token(Token::Type::IDENTIFIER, value);
}

auto Lexer::parse_number(char chr) -> Token {
  std::string value;
  value += chr;
  while (std::isdigit(chr = _source.next_char())) {
    if (_source.eof()) throw std::runtime_error("Unexpected end of file while parsing number\n");
    value += chr;
  }

  return Token(Token::Type::NUMBER, value);
}

auto Lexer::parse_string(char chr) -> Token {
  std::string value;
  while ((chr = _source.next_char()) != '"') {
    if (_source.eof()) throw std::runtime_error("Unexpected end of file while parsing string\n");
    value += chr;
  }

  return Token(Token::Type::STRING, value);
}

}  // namespace kuso