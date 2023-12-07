#pragma once

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

namespace kuso {
class Emitter {
  VIRTUAL(Emitter)
 public:
  virtual void emit() = 0;
};
}  // namespace kuso