/**
 * @file frontend.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2025-3-14
 *
 * @copyright Copyright 2025 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <belt/class_macros.hpp>
#include <map>

#include "parser/ast.hpp"

#include "type_system/type_id.hpp"
#include "type_system/type_system.hpp"
#include "type_system/types.hpp"

#include "context/variable.hpp"

#include "x64/addressing.hpp"
#include "x64/x64.hpp"

namespace kuso {
/**
   * @brief Handles the first pass of the code generator, collecting types and context information
   * 
   */
class Frontend {
  DEFAULT_CONSTRUCTIBLE(Frontend)
  DEFAULT_COPYABLE(Frontend)
  DEFAULT_MOVABLE(Frontend)
  DEFAULT_DESTRUCTIBLE(Frontend)

 public:
  [[nodiscard]] auto frontend_pass(const AST&) -> bool;

  [[nodiscard]] auto get_types() const -> TypeSystem const& { return _types; }

 private:
  TypeSystem _types;

  void generate_type(const AST::Type&);

  void pass_func(const AST::Func&);
  void pass_decl(const AST::Declaration&);
  void pass_call(const AST::Call&);
  void pass_main(const AST::Main&);
  void pass_expression(const AST::Expression&);
};
}  // namespace kuso

// struct FuncInfo {
//   int64_t                               size;
//   x64::Address                          stack;
//   std::map<std::string, Variable>       locals;
//   std::map<std::string, Variable>       params;
//   std::array<bool, x64::REGISTER_COUNT> dirtyRegs{false};
// };