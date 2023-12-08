#pragma once

#include <memory>
#include <stack>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "parser/ast.hpp"

#include "context.hpp"
#include "variables.hpp"

#include "x86/x86.hpp"

namespace kuso {
class Generator {
  NON_DEFAULT_CONSTRUCTIBLE(Generator)
  DEFAULT_DESTRUCTIBLE(Generator)
  DEFAULT_MOVABLE(Generator)
  NON_COPYABLE(Generator)

 public:
  explicit Generator(const std::filesystem::path& outputpath) : _outputFile(outputpath, std::ios_base::out) {}

  void generate(const AST&);

 private:
  belt::File          _outputFile;
  std::stack<Context> _contexts;

  void emit(const std::string&);
  void emit(x86::Op);
  void emit(x86::Op, x86::Register);
  void emit(x86::Op, x86::Source, x86::Destination);
  void emit(x86::Op, x86::Source, int);

  void generate(const AST::Statement&);
  void generate_assignment(const AST::Assignment&);
  void generate_declaration(const AST::Declaration&);

  [[nodiscard]] static auto get_identifier(const AST::Expression&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Terminal&) -> const std::string&;
  [[nodiscard]] static auto get_identifier(const AST::Declaration&) -> const std::string&;

  [[nodiscard]] static auto get_type(const AST::Declaration&) -> const std::string&;

  [[nodiscard]] auto context() -> Context& { return _contexts.top(); }
  [[nodiscard]] auto context() const -> const Context& { return _contexts.top(); }
};
}  // namespace kuso