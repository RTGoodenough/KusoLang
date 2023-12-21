/**
 * @file lexer.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <filesystem>
#include <map>
#include <vector>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "token.hpp"

namespace kuso {
/**
 * @brief Lexer class
 * 
 */
class Lexer {
  DEFAULT_CONSTRUCTIBLE(Lexer)
  DEFAULT_COPYABLE(Lexer)
  DEFAULT_DESTRUCTIBLE(Lexer)
  DEFAULT_MOVABLE(Lexer)

  using cstr_iter = std::string_view::const_iterator;

 public:
  std::string_view _source;
  cstr_iter        _iter{};
  int              _line{1};
  int              _col{1};

  [[nodiscard]] auto tokenize(const std::filesystem::path&) -> std::vector<Token>;
  [[nodiscard]] auto tokenize(const std::string&) -> std::vector<Token>;

  [[nodiscard]] auto by_token(const std::filesystem::path&) -> belt::Generator<Token>;
  [[nodiscard]] auto by_token(std::string) -> belt::Generator<Token>;

 private:
  [[nodiscard]] static auto is_keyword(const std::string&) -> bool;
  [[nodiscard]] static auto keywords() -> const std::map<std::string, Token::Type>&;
  [[nodiscard]] static auto replace_keyword_type(const std::string&) -> Token::Type;

  [[nodiscard]] auto replace_bool_equal() -> Token;
  [[nodiscard]] auto replace_gt_lt(bool) -> Token;
  [[nodiscard]] auto replace_not_equal() -> Token;

  void               skip_comments();
  [[nodiscard]] auto parse_identifier() -> Token;
  [[nodiscard]] auto parse_number() -> Token;
  [[nodiscard]] auto parse_string() -> Token;
  [[nodiscard]] auto parse_token() -> Token;
};
}  // namespace kuso