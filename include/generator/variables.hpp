#pragma once

#include <map>
#include <optional>

#include "x86/addressing.hpp"
#include "x86/x86.hpp"

namespace kuso {

struct Type_t {
  int                                       size{x86::Size::QWORD};
  std::optional<std::map<std::string, int>> offsets;

  [[nodiscard]] auto get_offset(const std::string& attribute) -> int {
    if (!offsets.has_value()) return 0;
    return offsets->at(attribute);
  }
};

struct Variable {
  int          type{0};
  x86::Address location;
};

struct String {
  std::string name;
  std::string value;
};
}  // namespace kuso