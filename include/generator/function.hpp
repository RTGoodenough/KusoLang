#pragma once

#include <memory>
#include <string>

#include "parser/ast.hpp"

namespace kuso {
struct Function {
  std::string                             label;
  std::reference_wrapper<const AST::Func> body;
};
}  // namespace kuso