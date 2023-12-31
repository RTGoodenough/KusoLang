#pragma once

#include "parser/ast.hpp"

#include "context.hpp"

namespace kuso {
class ContextPass {
 public:
  [[nodiscard]] auto pass(const AST&) -> bool;

  struct ContextError : public std::runtime_error {
    explicit ContextError(const std::string& what) : std::runtime_error(what) {}
  };

  [[nodiscard]] auto get_context(const std::string&) const -> const Context&;
  [[nodiscard]] auto get_context(const std::string&) -> Context&;

 private:
  std::string                    _current;
  std::map<std::string, Context> _contexts;

  void context_statement(const AST::Statement&);

  [[nodiscard]] auto current() -> Context&;
  auto               new_context(const std::string&) -> Context&;
  void               leave_context();

  void context_declaration(const AST::Declaration&);
  void context_if(const AST::If&);
  void context_while(const AST::While&);
  void context_func(const AST::Func&);
  void context_type(const AST::Type&);
  void context_return(const AST::Return&);
};
}  // namespace kuso
