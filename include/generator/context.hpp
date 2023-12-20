#pragma once

#include <map>
#include <stack>
#include <string>

#include "generator/variables.hpp"
#include "x64/addressing.hpp"

namespace kuso {
/**
 * @brief Holds information about the current context
 * 
 */
struct Context {
  int                             size;
  x64::Address                    stack;
  std::map<std::string, Variable> variables;
  int                             currVariable;
  std::map<std::string, int>      typeIDs;
  std::map<int, Type_t>           types;
};
}  // namespace kuso