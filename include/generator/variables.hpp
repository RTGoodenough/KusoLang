#pragma once

#include "x86/addressing.hpp"

namespace kuso {
struct Type_t {
  int size;
  // NOLINTNEXTLINE(google-explicit-constructor)
  // [[nodiscard]] operator int() const { return type; }
};

// TODO NEXT: get variable locations working
struct Variable {
  int          type{0};
  x86::Address location;
};
}  // namespace kuso