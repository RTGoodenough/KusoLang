#pragma once

#include <map>
#include <stack>
#include <string>

#include "generator/variables.hpp"
#include "x86/addressing.hpp"

namespace kuso {
struct Context {
  int                             size;
  x86::Address                    stack;
  std::map<std::string, Variable> variables;
  int                             currVariable;
  std::map<std::string, int>      typeIDs;
  std::map<int, Type_t>           types;
};
}  // namespace kuso