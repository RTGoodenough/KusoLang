

#include "generator/first_pass.hpp"

#include <belt/overload.hpp>

#include "logging/logging.hpp"

namespace kuso {

FirstPass::FirstPass() { _types.add_type("int", Type{x64::Size::QWORD, std::nullopt}); }

/**
 * @brief Handles the first pass of the types
 * 
 * @param ast AST to pass
 * @return true If the pass was successful
 * @return false If the pass was unsuccessful
 */
auto FirstPass::types_pass(const AST& ast) -> bool {
  try {
    bool hasMain = false;
    for (const auto& statement : ast) {
      belt::overloaded_visit(
          statement.statement, [&](const std::unique_ptr<AST::Type>& type) { generate_type(*type); },
          [&](const std::unique_ptr<AST::Declaration>&) {}, [&](const std::unique_ptr<AST::If>&) {},
          [&](const std::unique_ptr<AST::While>&) {}, [&](const std::unique_ptr<AST::Func>&) {},
          [&](const std::unique_ptr<AST::Return>&) {}, [&](const std::unique_ptr<AST::Exit>&) {},
          [&](const std::unique_ptr<AST::Assignment>&) {}, [&](const std::unique_ptr<AST::Print>&) {},
          [&](const std::unique_ptr<AST::Main>&) { hasMain = true; },
          [&](const std::unique_ptr<AST::Call>&) {}, [](std::nullptr_t) {});
    }

    if (!hasMain) {
      throw FirstPassException("No entry point found");
    }
  } catch (FirstPassException& e) {
    Logging::error(e.what());
    return false;
  }

  return true;
}

/**
 * @brief Handles the first pass of the functions
 * 
 * @param ast AST to pass
 * @return true If the pass was successful
 * @return false If the pass was unsuccessful
 */
auto FirstPass::function_pass(const AST& ast) -> bool {
  try {
    for (const auto& statement : ast) {
      belt::overloaded_visit(
          statement.statement, [&](const std::unique_ptr<AST::Type>&) {},
          [&](const std::unique_ptr<AST::Declaration>& decl) { pass_decl(*decl); },
          [&](const std::unique_ptr<AST::If>& ifStatement) { pass_expression(*ifStatement->condition); },
          [&](const std::unique_ptr<AST::While>& whileStatement) {
            pass_expression(*whileStatement->condition);
          },
          [&](const std::unique_ptr<AST::Func>& func) { pass_func(*func); },
          [&](const std::unique_ptr<AST::Return>&) {}, [&](const std::unique_ptr<AST::Exit>&) {},
          [&](const std::unique_ptr<AST::Assignment>& assignment) { pass_expression(*assignment->value); },
          [&](const std::unique_ptr<AST::Print>&) {},
          [&](const std::unique_ptr<AST::Main>& main) { pass_main(*main); },
          [&](const std::unique_ptr<AST::Call>&) {}, [](std::nullptr_t) {});
    }
  } catch (FirstPassException& e) {
    Logging::error(e.what());
    return false;
  }

  return true;
}

/**
 * @brief Adds a new type to the current context
 * 
 * @param type Type to create
 */
void FirstPass::generate_type(const AST::Type& type) {
  auto typeIter = get_type(type.name);
  if (typeIter.has_value()) {
    throw std::runtime_error("Multiple Declarations of " + type.name);
  }

  Type newType;

  if (!type.attributes.empty()) {
    newType.offsets = std::map<std::string, int>{};
    int currOffset = 0;

    for (const auto& attribute : type.attributes) {
      auto refType = _types.get_type(attribute.type);
      if (!refType.has_value()) {
        throw std::runtime_error("Unknown Type " + attribute.type);
      }

      newType.offsets.value()[attribute.name] = currOffset;
      currOffset += refType.value().get().size;
      newType.size += refType.value().get().size;
    }
  } else {
    newType.size = x64::Size::QWORD;
  }

  _types.add_type(type.name, newType);
}

/**
 * @brief Handles the first pass of a function
 * 
 * @param func Function to pass
 */
void FirstPass::pass_func(const AST::Func& func) {
  if (_functions.find(func.name) != _functions.end()) {
    throw FirstPassException("Multiple Declarations of " + func.name);
  }

  _currFunc = func.name;

  FuncInfo newFunc;
  newFunc.size = 0;
  newFunc.stack = x64::Address{x64::Address::Mode::INDIRECT_DISPLACEMENT, x64::Register::RSP, 0};

  size_t paramIndex = 0;
  for (const auto& arg : func.args) {
    auto typeID = _types.get_type_id(arg->type);
    if (!typeID.has_value()) {
      throw FirstPassException("Unknown Type " + arg->type);
    }

    auto typeIter = _types.get_type(arg->type);
    if (!typeIter.has_value()) {
      throw FirstPassException("Unknown Type " + arg->type);
    }

    auto reg = x64::parameter_reg(paramIndex);
    if (reg != x64::Register::NONE) {
      newFunc.params[arg->name] = Variable{typeID.value(), x64::Address{x64::Address::Mode::DIRECT, reg}};
    } else {
      newFunc.params[arg->name] = Variable{typeID.value(), newFunc.stack};
      newFunc.stack.disp += typeIter.value().get().size;
      newFunc.size += typeIter.value().get().size;
    }
  }

  _functions[func.name] = newFunc;

  for (const auto& statement : func.body) {
    belt::overloaded_visit(
        statement.statement, [&](const std::unique_ptr<AST::Type>&) {},
        [&](const std::unique_ptr<AST::Declaration>& decl) { pass_decl(*decl); },
        [&](const std::unique_ptr<AST::If>& ifStatement) { pass_expression(*ifStatement->condition); },
        [&](const std::unique_ptr<AST::While>& whileStatement) {
          pass_expression(*whileStatement->condition);
        },
        [&](const std::unique_ptr<AST::Func>&) {}, [&](const std::unique_ptr<AST::Return>&) {},
        [&](const std::unique_ptr<AST::Exit>&) {},
        [&](const std::unique_ptr<AST::Assignment>& assignment) { pass_expression(*assignment->value); },
        [&](const std::unique_ptr<AST::Print>&) {}, [&](const std::unique_ptr<AST::Main>&) {},
        [&](const std::unique_ptr<AST::Call>& call) { pass_call(*call); }, [](std::nullptr_t) {});
  }
}

/**
 * @brief Handles the first pass of the main function
 * 
 * @param main Main function to pass
 */
void FirstPass::pass_main(const AST::Main& main) {
  if (_functions.find("main") != _functions.end()) {
    throw FirstPassException("Multiple Declarations of main");
  }

  _currFunc = "main";

  FuncInfo newFunc;
  newFunc.size = 0;
  newFunc.stack = x64::Address{x64::Address::Mode::INDIRECT_DISPLACEMENT, x64::Register::RSP, 0};

  _functions["main"] = newFunc;

  for (const auto& statement : main.body) {
    belt::overloaded_visit(
        statement.statement, [&](const std::unique_ptr<AST::Type>&) {},
        [&](const std::unique_ptr<AST::Declaration>& decl) { pass_decl(*decl); },
        [&](const std::unique_ptr<AST::If>& ifStatement) { pass_expression(*ifStatement->condition); },
        [&](const std::unique_ptr<AST::While>& whileStatement) {
          pass_expression(*whileStatement->condition);
        },
        [&](const std::unique_ptr<AST::Func>&) {}, [&](const std::unique_ptr<AST::Return>&) {},
        [&](const std::unique_ptr<AST::Exit>&) {},
        [&](const std::unique_ptr<AST::Assignment>& assignment) { pass_expression(*assignment->value); },
        [&](const std::unique_ptr<AST::Print>&) {}, [&](const std::unique_ptr<AST::Main>&) {},
        [&](const std::unique_ptr<AST::Call>&) {}, [](std::nullptr_t) {});
  }
}

/**
 * @brief Handles the first pass of a call
 * 
 * @param call Call to pass
 */
void FirstPass::pass_call(const AST::Call& call) {
  for (const auto& arg : call.args) {
    pass_expression(*arg);
  }
}

/**
 * @brief Handles the first pass of an expression
 * 
 * @param expr Expression to pass
 */
void FirstPass::pass_expression(const AST::Expression& /*unused*/) {
  auto& context = _functions[_currFunc];
  // TODO(rolland): temporarily setting all registers to dirty on a call
  context.dirtyRegs.fill(true);
}

/**
 * @brief Handles the first pass of a declaration
 * 
 * @param decl Declaration to pass
 */
void FirstPass::pass_decl(const AST::Declaration& decl) {
  auto& context = _functions[_currFunc];

  auto typeID = _types.get_type_id(decl.type);
  if (!typeID.has_value()) {
    throw FirstPassException("Unknown Type " + decl.type);
  }
  auto typeIter = _types.get_type(decl.type);
  if (!typeIter.has_value()) {
    throw FirstPassException("Unknown Type " + decl.type);
  }

  context.locals[decl.name] = Variable{typeID.value(), context.stack};
  context.stack.disp += typeIter.value().get().size;
  context.size += typeIter.value().get().size;
}
}  // namespace kuso