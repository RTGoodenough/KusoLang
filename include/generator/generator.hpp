/**
 * @file generator.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <memory>
#include <stack>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "generator/context_pass.hpp"
#include "generator/first_pass.hpp"
#include "parser/ast.hpp"

#include "context.hpp"
#include "function.hpp"
#include "variables.hpp"

#include "x64/addressing.hpp"
#include "x64/x64.hpp"

namespace kuso {
/**
 * @brief Code generator class
 * 
 */
class Generator {
  NON_DEFAULT_CONSTRUCTIBLE(Generator)
  DEFAULT_DESTRUCTIBLE(Generator)
  DEFAULT_MOVABLE(Generator)
  NON_COPYABLE(Generator)

 public:
  explicit Generator(const std::filesystem::path& outputpath);

  void generate(const AST&);

 private:
  belt::File _outputFile;

  FirstPass _firstpass;

  std::stack<Context> _contexts;

  std::stack<std::string>         _currentFunction;
  std::map<std::string, Function> _functions;

  std::map<std::string, std::string> _string_names;
  std::map<std::string, std::string> _string_values;

  std::string _output_code;

  size_t _label_count{0};

  bool _exprInReg{false};

  void init_context();

  void enter_context(int64_t);
  void enter_context(const std::string&);
  void leave_context();

  [[nodiscard]] auto new_label() -> std::string;

  void push(x64::Register);
  void push(x64::Address);
  void push(x64::Literal);
  void pop(x64::Register);
  void emit(x64::Op);
  void emit(const std::string&);
  void emit(x64::Op, const std::string&);
  void emit(x64::Op, x64::Register);
  void emit(x64::Op, x64::Address);
  void emit(x64::Op, x64::Size, x64::Register);
  void emit(x64::Op, x64::Size, x64::Address);
  void emit(x64::Op, x64::Size, x64::Literal);
  void emit(x64::Op, x64::Address, x64::Address);
  void emit(x64::Op, x64::Register, x64::Address);
  void emit(x64::Op, x64::Register, x64::Register);
  void emit(x64::Op, x64::Address, x64::Register);
  void emit(x64::Op, x64::Register, const std::string&);
  void emit(x64::Op, x64::Literal);
  void emit(x64::Op, x64::Literal, x64::Literal);
  void emit(x64::Op, x64::Address, x64::Literal);
  void emit(x64::Op, x64::Register, x64::Literal);

  void pull_comparison_result(AST::BinaryOp);

  void generate(const AST::Statement&);
  void generate_assignment(const AST::Assignment&);
  void generate_declaration(const AST::Declaration&);
  void generate_exit(const AST::Exit&);

  void generate_if(const AST::If&);

  void generate_while(const AST::While&);

  void generate_expression(const AST::Expression&);
  void generate_expression(const AST::Terminal&);
  void generate_expression(const AST::Equality&);
  void generate_expression(const AST::Comparison&);
  void generate_expression(const AST::Term&);
  void generate_expression(const AST::Factor&);
  void generate_expression(const AST::Unary&);
  void generate_expression(const AST::Primary&);
  void generate_expression(const AST::Variable&);
  void generate_expression(const Token&);

  void generate_func(const AST::Func&);
  void generate_inline_asm(const AST::ASM&);

  void generate_main(const AST::Main&);
  void generate_call(const AST::Call&);
  void generate_parameters(const AST::Call&);
  void generate_return(const AST::Return&);

  void generate_string(const AST::String&);

  [[nodiscard]] auto get_location(const AST::Variable&) -> x64::Address;

  [[nodiscard]] static auto get_identifier(const AST::Terminal&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Declaration&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Assignment&) -> const std::string&;
  [[nodiscard]] static auto get_decl_type(const AST::Declaration&) -> const std::string&;

  [[nodiscard]] auto get_check_func_info(const std::string&) -> const FirstPass::FuncInfo&;
  [[nodiscard]] auto get_check_type(const std::string&) -> Type&;
  [[nodiscard]] auto get_check_type(TypeID) -> Type&;

  [[nodiscard]] inline auto context() -> Context& { return _contexts.top(); }
  [[nodiscard]] inline auto context() const -> const Context& { return _contexts.top(); }
};
}  // namespace kuso