#pragma once

#include "x86/addressing.hpp"

namespace kuso {
struct Type_t {
  int size;
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