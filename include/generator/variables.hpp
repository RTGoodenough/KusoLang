#pragma once

struct Type_t {
  int type;
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] operator int() const { return type; }
};

struct Variable {
  int type;
  int location;
};