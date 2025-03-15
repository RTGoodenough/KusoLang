#pragma once

#include <stdexcept>

namespace kuso {
class SemanticException : public std::runtime_error {
 public:
  explicit SemanticException(const std::string& what) : std::runtime_error(what)
  {
  }
};
}  // namespace kuso