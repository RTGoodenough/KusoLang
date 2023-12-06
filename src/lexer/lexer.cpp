

#include <iostream>

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

namespace kuso {

auto Lexer::by_token() -> belt::Generator<std::pair<Token, Token>> {
  Token token = parse_token();

  while (_source.eof()) {
    Token    lookahead = parse_token();
    co_yield std::make_pair(token, lookahead);
    token = lookahead;
    lookahead = parse_token();
  }

  co_return std::make_pair(Token(Token::Type::END_OF_FILE), Token(Token::Type::END_OF_FILE));
}

auto Lexer::parse_token() -> Token {
  char chr = _source.next_char();
  while (std::isspace(chr)) {
    chr = _source.next_char();
  }
  if (chr == '/') {
    chr = skip_comments();
  }

  if (_source.eof()) return Token(Token::Type::END_OF_FILE);

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