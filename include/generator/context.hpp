#pragma once

#include <map>
#include <string>

#include "generator/variables.hpp"
#include "x86/addressing.hpp"

namespace kuso {
struct Context {
  x86::Address                    stack;
  std::map<std::string, Variable> variables;
  std::map<std::string, int>      typeIDs;
  std::map<int, Type_t>           types;
};
}  // namespace kuso