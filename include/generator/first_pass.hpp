#pragma once

#include <belt/class_macros.hpp>
#include "generator/context.hpp"
#include "generator/types.hpp"
#include "parser/ast.hpp"
#include "x64/x64.hpp"

namespace kuso {
/**
  * @brief Handles the first pass of the code generator, collecting types and context information
  * 
  */
class FirstPass {
  NON_DEFAULT_DEFAULT_CONSTRUCTIBLE(FirstPass)
  DEFAULT_COPYABLE(FirstPass)
  DEFAULT_MOVABLE(FirstPass)
  DEFAULT_DESTRUCTIBLE(FirstPass)

 public:
  struct FuncInfo {
    int64_t                               size;
    x64::Address                          stack;
    std::map<std::string, Variable>       locals;
    std::map<std::string, Variable>       params;
    std::array<bool, x64::REGISTER_COUNT> dirtyRegs{false};
  };

  [[nodiscard]] auto types_pass(const AST&) -> bool;
  [[nodiscard]] auto function_pass(const AST&) -> bool;

  [[nodiscard]] auto get_functions() -> std::map<std::string, FuncInfo>& { return _functions; }
  [[nodiscard]] auto get_function(const std::string& name)
      -> std::optional<std::reference_wrapper<FuncInfo>> {
    if (_functions.find(name) == _functions.end()) {
      return std::nullopt;
    }

    return _functions.at(name);
  }

  [[nodiscard]] auto get_types() -> TypeContainer& { return _types; }
  [[nodiscard]] auto get_type_id(const std::string& name) -> std::optional<TypeID> {
    return _types.get_type_id(name);
  }

  [[nodiscard]] auto get_type(const std::string& name) -> std::optional<std::reference_wrapper<Type>> {
    return _types.get_type(name);
  }

  [[nodiscard]] auto get_type(TypeID tid) -> std::optional<std::reference_wrapper<Type>> {
    return _types.get_type(tid);
  }

  class FirstPassException : public std::runtime_error {
   public:
    explicit FirstPassException(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  TypeContainer                   _types;
  std::map<std::string, FuncInfo> _functions;
  std::string                     _currFunc;

  void generate_type(const AST::Type&);

  void pass_func(const AST::Func&);
  void pass_decl(const AST::Declaration&);
  void pass_call(const AST::Call&);
  void pass_main(const AST::Main&);
  void pass_expression(const AST::Expression&);
};
}  // namespace kuso