#pragma once

#include <belt/class_macros.hpp>

namespace kuso {
class Emitter {
  VIRTUAL(Emitter)
 public:
  enum class Type {
    ASM,
  };

  virtual void emit_setup() = 0;
  virtual void finish() = 0;
};
}  // namespace kuso