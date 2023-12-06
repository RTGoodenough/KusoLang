#pragma once

#include <filesystem>

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
  explicit Lexer(const std::filesystem::path& path) : _source(path, std::ios::in) {}

  [[nodiscard]] auto by_token() -> belt::Generator<std::pair<Token, Token>>;

 private:
  belt::File _source;

  [[nodiscard]] auto skip_comments() -> char;

  [[nodiscard]] auto parse_identifier(char) -> Token;
  [[nodiscard]] auto parse_number(char) -> Token;
  [[nodiscard]] auto parse_string(char) -> Token;
  [[nodiscard]] auto parse_token() -> Token;
};
}  // namespace kuso