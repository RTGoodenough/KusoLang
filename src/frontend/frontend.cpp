/*
* @file frontend.hpp
* @author Rolland Goodenough (goodenoughr@gmail.com)
* @date 2025-3-14
*
* @copyright Copyright 2025 Rolland Goodenough
*
* This file is part of kuso which is released under the MIT License
* See file LICENSE for the full License
*/

#include "frontend/frontend.hpp"

#include <belt/overload.hpp>

#include "exceptions/semantic.hpp"

#include "logging/logging.hpp"

namespace kuso {
auto Frontend::frontend_pass(const AST& ast) -> bool
{
  try {
    bool hasMain = false;

    for ( const auto& statement : ast ) {
      belt::overloaded_visit(
          statement.statement,
          [&](const std::unique_ptr<AST::Type>& type) { generate_type(*type); },
          [&](const std::unique_ptr<AST::Declaration>&) {},
          [&](const std::unique_ptr<AST::If>&) {},
          [&](const std::unique_ptr<AST::While>&) {},
          [&](const std::unique_ptr<AST::ASM>&) {},
          [&](const std::unique_ptr<AST::Func>&) {},
          [&](const std::unique_ptr<AST::Return>&) {},
          [&](const std::unique_ptr<AST::Exit>&) {},
          [&](const std::unique_ptr<AST::Assignment>&) {},
          [&](const std::unique_ptr<AST::Main>&) {},
          [&](const std::unique_ptr<AST::Call>&) {}, [](std::nullptr_t) {});
    }

    if ( ! hasMain ) {
      throw SemanticException("No entry point found");
    }
  }
  catch ( std::runtime_error& e ) {
    Logging::error(e.what());
    return false;
  }

  return true;
}

void Frontend::generate_type(const AST::Type& type)
{
  Type newType;

  if ( ! type.attributes.empty() ) {
    for ( auto const& attr : type.attributes ) {
      newType.add_attribute(attr.name, attr.type, _types);
    }
  }
  else {
    newType.size = x64::Size::BYTE;
  }

  _types.add_type(type.name, newType);
}
}  // namespace kuso