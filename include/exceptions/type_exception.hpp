#pragma once

#include <stdexcept>
namespace kuso {
class TypeException : public std::runtime_error {
 public:
  explicit TypeException(const std::string& what) : std::runtime_error(what) {}
};
}  // namespace kuso