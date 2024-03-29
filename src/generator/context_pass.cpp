/**
 * @file context_pass.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-29
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include "generator/context_pass.hpp"

#include <iostream>

#include <belt/overload.hpp>

#include "x64/addressing.hpp"
#include "x64/x64.hpp"

namespace kuso {
auto ContextPass::pass(const AST& ast) -> bool {
  _contexts.emplace("global",
                    Context{.size = 0,
                            .stack = x64::Address{x64::Address::Mode::DIRECT, x64::Register::RSP, 0},
                            .variables = {},
                            .currVariable = 2});
  try {
    for (const auto& statement : ast) {
      context_statement(statement);
    }
  } catch (const ContextPass::ContextError& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }

  return true;
}

void ContextPass::context_statement(const AST::Statement& statement) {
  belt::overloaded_visit(
      statement.statement,
      [&](const std::unique_ptr<AST::Declaration>& declaration) { context_declaration(*declaration); },
      [&](const std::unique_ptr<AST::If>& ifStatement) { context_if(*ifStatement); },
      [&](const std::unique_ptr<AST::Type>& type) { context_type(*type); },
      [&](const std::unique_ptr<AST::While>& whileStatement) { context_while(*whileStatement); },
      [&](const std::unique_ptr<AST::Func>& func) { context_func(*func); },
      [&](const std::unique_ptr<AST::Return>& return_) { context_return(*return_); },
      [&](const std::unique_ptr<AST::ASM>&) {}, [&](const std::unique_ptr<AST::Exit>&) {},
      [&](const std::unique_ptr<AST::Assignment>&) {}, [&](const std::unique_ptr<AST::Call>&) {},
      [](std::nullptr_t) {});
}

void ContextPass::context_declaration(const AST::Declaration& declaration) {
  auto& context = current();

  auto  typeID = get_type_id(declaration.type);
  auto& type = get_type(typeID);

  auto size = type.size;
  context.variables[declaration.name] =
      Variable{size, x64::Address{x64::Address::Mode::INDIRECT, x64::Register::RSP, context.size}};
  context.size += size;
}

void ContextPass::context_if(const AST::If& if_) {
  for (const auto& statement : if_.body) {
    context_statement(statement);
  }

  for (const auto& statement : if_.elseBody) {
    context_statement(statement);
  }
}

void ContextPass::context_while(const AST::While& while_) {
  for (const auto& statement : while_.body) {
    context_statement(statement);
  }
}

void ContextPass::context_func(const AST::Func& func) {
  auto& context = new_context(func.name);

  context.size = 0;
  context.stack = x64::Address{x64::Address::Mode::DIRECT, x64::Register::RSP, 0};
  context.currVariable = 0;

  for (const auto& param : func.args) {
    auto size = context.types[context.typeIDs[param->name]].size;
    context.variables[param->name] =
        Variable{size, x64::Address{x64::Address::Mode::INDIRECT, x64::Register::RSP, context.size}};
    context.size += size;
  }

  for (const auto& statement : func.body) {
    context_statement(statement);
  }

  leave_context();
}

void ContextPass::context_type(const AST::Type&) {}

void ContextPass::context_return(const AST::Return&) { leave_context(); }

auto ContextPass::get_context(const std::string& name) const -> const Context& {
  auto iter = _contexts.find(name);
  if (iter == _contexts.end()) {
    throw ContextError(fmt::format("Unknown Context {}", name));
  }

  return iter->second;
}
auto ContextPass::get_context(const std::string& name) -> Context& {
  auto iter = _contexts.find(name);
  if (iter == _contexts.end()) {
    throw ContextError(fmt::format("Unknown Context {}", name));
  }

  return iter->second;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% HELPERS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Returns the type with the given typeID
 * 
 * @param typeID ID of the type to get
 * @return Type_t& Type with the given typeID
 */
auto ContextPass::get_type(int typeID) -> Type_t& {
  auto typeIter = current().types.find(typeID);
  if (typeIter == current().types.end()) {
    throw std::runtime_error("Unknown Type " + std::to_string(typeID));
  }

  return typeIter->second;
}

/**
 * @brief Returns the typeID of the given type
 * 
 * @param type Type to get the ID of
 * @return int ID of the given type
 */
auto ContextPass::get_type_id(const std::string& type) -> int {
  auto typeIter = current().typeIDs.find(type);
  if (typeIter == current().typeIDs.end()) {
    throw std::runtime_error("Unknown Type " + type);
  }

  return typeIter->second;
}

auto ContextPass::new_context(const std::string& name) -> Context& {
  auto iter = _contexts.find(name);
  if (iter != _contexts.end()) {
    throw ContextError(fmt::format("Context {} already exists", name));
  }

  auto& cntx = _contexts[_current];
  _current = name;
  _contexts[_current] = Context{};

  return cntx;
}

void ContextPass::leave_context() {
  if (_current == "global") {
    throw ContextError("Cannot leave global context");
  }

  _current = "global";
}

auto ContextPass::current() -> Context& { return _contexts[_current]; }

}  // namespace kuso