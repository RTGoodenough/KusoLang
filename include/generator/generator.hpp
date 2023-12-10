#pragma once

#include <memory>
#include <stack>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "parser/ast.hpp"

#include "context.hpp"
#include "variables.hpp"

#include "x86/addressing.hpp"
#include "x86/x86.hpp"

namespace kuso {
class Generator {
  NON_DEFAULT_CONSTRUCTIBLE(Generator)
  DEFAULT_DESTRUCTIBLE(Generator)
  DEFAULT_MOVABLE(Generator)
  NON_COPYABLE(Generator)

 public:
  explicit Generator(const std::filesystem::path& outputpath);

  void generate(const AST&);

 private:
  belt::File          _outputFile;
  std::stack<Context> _contexts;

  std::vector<std::string> _labels;

  std::map<std::string, std::string> _string_names;
  std::map<std::string, std::string> _string_values;

  std::string _output_code;
  std::string _output_data;

  void init_context();
  void init_data();

  void add_data(const std::string&, const std::string&);
  void emit(x86::Op);
  void emit(const std::string&);
  void emit(x86::Op, const std::string&);
  void emit(x86::Op, x86::Register);
  void emit(x86::Op, x86::Address, x86::Address);
  void emit(x86::Op, x86::Register, x86::Address);
  void emit(x86::Op, x86::Register, x86::Register);
  void emit(x86::Op, x86::Address, x86::Register);
  void emit(x86::Op, x86::Register, const std::string&);
  void emit(x86::Op, x86::Literal);
  void emit(x86::Op, x86::Address, x86::Literal);
  void emit(x86::Op, x86::Register, x86::Literal);

  void generate(const AST::Statement&);
  void generate_assignment(const AST::Assignment&);
  void generate_declaration(const AST::Declaration&);
  void generate_return(const AST::Return&);
  void generate_exit(const AST::Exit&);
  void generate_print(const AST::Print&);
  void generate_if(const AST::If&);

  void generate_expression(const AST::Expression&);
  void generate_expression(const AST::Terminal&);
  void generate_string(const AST::String&);

  void new_context();
  void leave_context();

  [[nodiscard]] auto get_type_id(const std::string&) -> int;
  [[nodiscard]] auto get_type(int) -> Type_t;

  [[nodiscard]] static auto get_identifier(const AST::Expression&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Terminal&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Declaration&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Assignment&) -> const std::string&;
  [[nodiscard]] static auto get_decl_type(const AST::Declaration&) -> const std::string&;

  [[nodiscard]] inline auto context() -> Context& { return _contexts.top(); }
  [[nodiscard]] inline auto context() const -> const Context& { return _contexts.top(); }
};
}  // namespace kuso