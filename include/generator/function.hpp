/**
 * @file function.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-29
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <memory>
#include <string>

#include "parser/ast.hpp"

namespace kuso {
struct Function {
  std::string                             label;
  std::reference_wrapper<const AST::Func> body;
  size_t                                  argCnt;
};
}  // namespace kuso