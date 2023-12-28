/**
 * @file context.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

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
  int64_t                         size;
  x64::Address                    stack;
  std::map<std::string, Variable> variables;
  int                             currVariable;
  std::map<std::string, int>      typeIDs;
  std::map<int, Type_t>           types;
};
}  // namespace kuso