#pragma once

#include <map>
#include <string>

#include "generator/variables.hpp"

namespace kuso {
struct Context {
  std::map<std::string, Variable> variables;
  std::map<std::string, Type_t>   types;
};
}  // namespace kuso