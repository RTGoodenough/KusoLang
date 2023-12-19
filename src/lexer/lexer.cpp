
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

#include <fmt/format.h>
#include <cwctype>

namespace kuso {

// TODO(rolland): line column points to the end of the token

auto Lexer::by_token() -> belt::Generator<Token> {
  while (true) {
    if (_source.eof()) {
      co_yield Token(Token::Type::END_OF_FILE, _line, _column);
    }
    co_yield parse_token();
  }
}

auto Lexer::parse_token() -> Token {
  if (_source.eof()) {
    return Token(Token::Type::END_OF_FILE, _line, _column);
  }

  char chr = _source.next_char();
  ++_column;
  while (std::isspace(chr)) {
    if (chr == '\n') {
      ++_line;
      _column = 1;
    }
    chr = _source.next_char();
    ++_column;
  }
  while (chr == '/') {
    chr = skip_comments();
  }

  while (std::iswspace(chr)) {
    if (chr == '\n') {
      ++_line;
      _column = 1;
    }

    chr = _source.next_char();
    ++_column;
  }

  if (_source.eof()) {
    return Token(Token::Type::END_OF_FILE, _line, _column);
  }

  switch (chr) {
    case ';':
      return Token(Token::Type::SEMI_COLON, _line, _column);
      break;
    case ',':
      return Token(Token::Type::COMMA, _line, _column);
      break;
    case '(':
      return Token(Token::Type::OPEN_PAREN, _line, _column);
      break;
    case ')':
      return Token(Token::Type::CLOSE_PAREN, _line, _column);
      break;
    case '{':
      return Token(Token::Type::OPEN_BRACE, _line, _column);
      break;
    case '}':
      return Token(Token::Type::CLOSE_BRACE, _line, _column);
      break;
    case '[':
      return Token(Token::Type::OPEN_BRACKET, _line, _column);
      break;
    case ']':
      return Token(Token::Type::CLOSE_BRACKET, _line, _column);
      break;
    case ':':
      return Token(Token::Type::COLON, _line, _column);
      break;
    case '.':
      return Token(Token::Type::DOT, _line, _column);
      break;
    case '#':
      return Token(Token::Type::HASH, _line, _column);
      break;
    case '@':
      return Token(Token::Type::AT, _line, _column);
      break;
    case '&':
      return Token(Token::Type::AMPERSAND, _line, _column);
      break;
    case '*':
      return Token(Token::Type::ASTERISK, _line, _column);
      break;
    case '+':
      return Token(Token::Type::PLUS, _line, _column);
      break;
    case '-':
      if (_source.peek_char() == '>') {
        chr = _source.next_char();
        ++_column;
        return Token(Token::Type::ARROW, _line, _column);
      } else {
        return Token(Token::Type::MINUS, _line, _column);
      }
      break;
    case '%':
      return Token(Token::Type::PERCENT, _line, _column);
      break;
    case '^':
      return Token(Token::Type::CARET, _line, _column);
      break;
    case '~':
      return Token(Token::Type::TILDE, _line, _column);
      break;
    case '!':
      return replace_not_equal();
      break;
    case '?':
      return Token(Token::Type::QUESTION, _line, _column);
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
      return Token(Token::Type::SLASH, _line, _column);
      break;
    case '|':
      return Token(Token::Type::PIPE, _line, _column);
      break;
    case '"':
      return parse_string();
      break;
    case '\'':
      return Token(Token::Type::SINGLE_QUOTE, _line, _column);
      break;
    case '\\':
      return Token(Token::Type::BACKSLASH, _line, _column);
      break;
    case '`':
      return Token(Token::Type::BACKTICK, _line, _column);
      break;
    case '$':
      return Token(Token::Type::DOLLAR, _line, _column);
      break;
    case '_':
      return Token(Token::Type::UNDERSCORE, _line, _column);
      break;
    default: {
      if (std::isalpha(chr)) {
        return parse_identifier(chr);
      }
      if (std::isdigit(chr)) {
        return parse_number(chr);
      }
      throw std::runtime_error(fmt::format("Invalid character: {}", chr));
    }
  }
}

auto Lexer::skip_comments() -> char {
  if (_source.peek_char() == '/') {
    while (_source.next_char() != '\n') {
    }
    ++_line;
    _column = 1;
  } else if (_source.peek_char() == '*') {
    _source.next_char();
    ++_column;
    while (true) {
      if (_source.eof()) {
        throw std::runtime_error("Unterminated comment");
      }
      if (_source.peek_char() == '*') {
        _source.next_char();
        ++_column;
        if (_source.peek_char() == '/') {
          _source.next_char();
          ++_column;
          break;
        }
      }
      if (_source.peek_char() == '\n') {
        ++_line;
        _column = 1;
      }
      _source.next_char();
      ++_column;
    }
  }

  ++_column;
  return _source.next_char();
}

auto Lexer::parse_identifier(char chr) -> Token {
  std::string value;
  value += chr;
  while (std::isalnum(_source.peek_char()) && !_source.eof()) {
    value += _source.next_char();
    ++_column;
  }
  auto type = replace_keyword_type(value);
  if (type == Token::Type::IDENTIFIER) {
    return Token(type, _line, _column, value);
  }

  return Token(type, _line, _column);
}

auto Lexer::parse_number(char chr) -> Token {
  std::string value;
  value += chr;
  while (std::isdigit(_source.peek_char()) && !_source.eof()) {
    value += _source.next_char();
    ++_column;
  }

  return Token(Token::Type::NUMBER, _line, _column, value);
}

auto Lexer::parse_string() -> Token {
  std::string value = "\"";
  while (_source.peek_char() != '"' && !_source.eof()) {
    value += _source.next_char();
    ++_column;
  }

  if (_source.eof()) {
    throw std::runtime_error("Unterminated string");
  }
  value += _source.next_char();
  return Token(Token::Type::STRING, _line, _column, value);
}

auto Lexer::replace_bool_equal() -> Token {
  if (_source.peek_char() == '=') {
    _source.next_char();
    ++_column;
    return Token(Token::Type::BOOL_EQUAL, _line, _column);
  }

  return Token(Token::Type::EQUAL, _line, _column);
}

auto Lexer::replace_gt_lt(bool greater) -> Token {
  if (_source.peek_char() == '=') {
    _source.next_char();
    ++_column;
    if (greater) {
      return Token(Token::Type::GREATER_THAN_EQUAL, _line, _column);
    }
    return Token(Token::Type::LESS_THAN_EQUAL, _line, _column);
  }

  if (greater) {
    return Token(Token::Type::GREATER_THAN, _line, _column);
  }
  return Token(Token::Type::LESS_THAN, _line, _column);
}

auto Lexer::replace_not_equal() -> Token {
  if (_source.peek_char() == '=') {
    _source.next_char();
    ++_column;
    return Token(Token::Type::NOT_EQUAL, _line, _column);
  }

  return Token(Token::Type::EXCLAMATION, _line, _column);
}

auto Lexer::is_keyword(const std::string& value) -> bool {
  return keywords().find(value) != keywords().end();
}

auto Lexer::replace_keyword_type(const std::string& value) -> Token::Type {
  auto iter = keywords().find(value);
  if (iter != keywords().end()) {
    return iter->second;
  }

  return Token::Type::IDENTIFIER;
}

auto Lexer::keywords() -> const std::map<std::string, Token::Type>& {
  static const std::map<std::string, Token::Type> KEYWORDS{
      {"if", Token::Type::IF},       {"else", Token::Type::ELSE},     {"for", Token::Type::FOR},
      {"while", Token::Type::WHILE}, {"return", Token::Type::RETURN}, {"exit", Token::Type::EXIT},
      {"print", Token::Type::PRINT}, {"type", Token::Type::TYPE},     {"while", Token::Type::WHILE},
  };
  return KEYWORDS;
}
}  // namespace kuso