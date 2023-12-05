#pragma once

#include <filesystem>
#include <fstream>

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

  [[nodiscard]] auto eof() const -> bool { return _file.eof(); }
  [[nodiscard]] auto peek_char() -> int { return _file.peek(); }
  [[nodiscard]] auto next_char() -> char {
    char chr = '\0';
    _file.get(chr);
    _eof = _file.eof();
    return chr;
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