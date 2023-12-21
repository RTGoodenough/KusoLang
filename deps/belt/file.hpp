#pragma once

#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>

#include "class_macros.hpp"
#include "generator.hpp"

namespace belt {
class File {
  NON_DEFAULT_CONSTRUCTIBLE(File)
  NON_COPYABLE(File)
  DEFAULT_DESTRUCTIBLE(File)
  MOVABLE(File)

 public:
  explicit File(const std::filesystem::path& path, const std::ios::openmode& mode = std::ios::in)
      : _path(path), _mode(mode), _file(path, mode) {}

  [[nodiscard]] auto is_open() const noexcept -> bool { return _file.is_open(); }
  [[nodiscard]] auto eof() const -> bool { return _file.eof(); }
  [[nodiscard]] auto peek_char() -> int { return _file.peek(); }
  auto               next_char() -> char {
                  char chr = '\0';
                  _file.get(chr);
                  _eof = _file.eof();
                  return chr;
  }

  [[nodiscard]] auto read() -> std::string {
    std::string str;
    char        chr = '\0';
    while (_file.get(chr)) {
      str += chr;
    }

    return str;
  }
  [[nodiscard]] auto read_line() -> std::string {
    std::string line;
    std::getline(_file, line);
    return line;
  }
  [[nodiscard]] auto read_hex() -> std::uint8_t {
    std::uint8_t hex = 0;
    _file >> std::hex >> hex;
    return hex;
  }

  void write_ln(const std::string& str) { _file << str << '\n'; }
  void write(const std::string& str) { _file << str; }
  void write(char chr) { _file << chr; }
  void write_hex(std::uint8_t hex) { _file << std::hex << hex; }
  void write_hex(std::initializer_list<std::uint8_t> values) {
    for (const auto& value : values) {
      write_hex(value);
    }
  }

  [[nodiscard]] auto by_char() -> belt::Generator<char> {
    char chr = '\0';
    while (_file.get(chr)) {
      co_yield chr;
    }

    co_return '\0';
  }
  [[nodiscard]] auto by_line() -> belt::Generator<std::string> {
    std::string line;
    while (std::getline(_file, line)) {
      co_yield line;
    }

    co_return "";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path& { return _path; }
  [[nodiscard]] auto mode() const noexcept -> const std::ios::openmode& { return _mode; }
  [[nodiscard]] auto stream() const noexcept -> const std::fstream& { return _file; }

 private:
  std::filesystem::path _path;
  std::ios::openmode    _mode;
  std::fstream          _file;
  bool                  _eof = false;
};
};  // namespace belt