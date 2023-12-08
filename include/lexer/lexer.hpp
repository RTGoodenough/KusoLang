#pragma once

#include <filesystem>
#include <map>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "token.hpp"

namespace kuso {
class Lexer {
  NON_DEFAULT_CONSTRUCTIBLE(Lexer)
  NON_COPYABLE(Lexer)

  DEFAULT_DESTRUCTIBLE(Lexer)
  DEFAULT_MOVABLE(Lexer)

 public:
  explicit Lexer(const std::filesystem::path& path) : _source(path, std::ios::in) {
    if (!_source.is_open()) {
      throw std::runtime_error("Could not open file: " + path.string());
    }
  }

  [[nodiscard]] auto by_token() -> belt::Generator<Token>;

 private:
  belt::File _source;
  int        _line{1};
  int        _column{1};

  [[nodiscard]] static auto is_keyword(const std::string&) -> bool;
  [[nodiscard]] static auto keywords() -> const std::map<std::string, Token::Type>&;
  [[nodiscard]] static auto replace_keyword_type(const std::string&) -> Token::Type;

  [[nodiscard]] auto skip_comments() -> char;
  [[nodiscard]] auto parse_identifier(char) -> Token;
  [[nodiscard]] auto parse_number(char) -> Token;
  [[nodiscard]] auto parse_string(char) -> Token;
  [[nodiscard]] auto parse_token() -> Token;
};
}  // namespace kuso