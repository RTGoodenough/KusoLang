/**
 * @file variables.hpp
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
#include <optional>

#include "x64/addressing.hpp"
#include "x64/x64.hpp"

#include "types.hpp"

namespace kuso {

/**
 * @brief Holds information about a variable
 * 
 */
struct Variable {
  TypeID       type{0};
  x64::Address location;
};

/**
 * @brief Holds a string literal
 * 
 */
struct String {
  std::string name;
  std::string value;
};
}  // namespace kuso