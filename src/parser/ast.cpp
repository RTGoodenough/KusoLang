/**
 * @file ast.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include "parser/ast.hpp"

#include <fmt/format.h>
#include <belt/class_macros.hpp>
#include <belt/overload.hpp>

namespace kuso {
/**
 * @brief Converts the AST to a string
 * 
 * @return std::string
 */
auto AST::to_string() const -> std::string {
  std::string result;
  for (const auto& statement : _statements) {
    belt::overloaded_visit(
        statement.statement,
        [&result](const std::unique_ptr<Assignment>& assignment) { result += assignment->to_string(0); },
        [&result](const std::unique_ptr<Type>& type) { result += type->to_string(0); },
        [&result](const std::unique_ptr<If>& if_) { result += if_->to_string(0); },
        [&result](const std::unique_ptr<Print>& print) { result += print->value->to_string(0); },
        [&result](const std::unique_ptr<Return>& return_) { result += return_->value->to_string(0); },
        [&result](const std::unique_ptr<Exit>& exit) { result += exit->value->to_string(0); },
        [&result](const std::unique_ptr<Call>& call) { result += call->to_string(0); },
        [&result](const std::unique_ptr<Func>& func) { result += func->to_string(0); },
        [&result](const std::unique_ptr<Declaration>& declaration) { result += declaration->to_string(0); },
        [&result](const std::unique_ptr<While>& while_) { result += while_->to_string(0); },
        [&result](std::nullptr_t) { result += "null\n"; });
  }
  return result;
}

/**
 * @brief Returns a reference to the ast statements vector
 * 
 * @return const std::vector<Statement>& 
 */
auto AST::statements() const -> const std::vector<Statement>& { return _statements; }

/**
 * @brief Adds a statement to the AST
 * 
 * @param statement 
 */
void AST::add_statement(Statement&& statement) { _statements.emplace_back(std::move(statement)); }

/**
 * @brief Converts a binary operator to a string
 * 
 * @param type binary operator
 */
auto AST::op_to_string(BinaryOp type) -> std::string {
  static const std::unordered_map<BinaryOp, std::string> OPERATORS{
      {BinaryOp::ADD, "+"}, {BinaryOp::SUB, "-"}, {BinaryOp::MUL, "*"},  {BinaryOp::DIV, "/"},
      {BinaryOp::MOD, "%"}, {BinaryOp::POW, "^"}, {BinaryOp::GTE, ">="}, {BinaryOp::LTE, "<="},
      {BinaryOp::GT, ">"},  {BinaryOp::LT, "<"},  {BinaryOp::EQ, "=="},  {BinaryOp::NEQ, "!="},
  };
  auto oper = OPERATORS.find(type);
  if (oper == OPERATORS.end()) {
    // TODO(rolland): handle this error
    return "!";
  }

  return oper->second;
}

auto AST::begin() -> iterator { return _statements.begin(); }
auto AST::end() -> iterator { return _statements.end(); }
auto AST::begin() const -> const_iterator { return _statements.begin(); }
auto AST::end() const -> const_iterator { return _statements.end(); }

// ----------------------------------------- NODE TYPES ------------------------------------------------------

/**
 * @brief returns the string representation of the assignment
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Assignment::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}", "", indent) + dest->name + " = " +
         (value ? value->to_string(indent + 1) : "") + '\n';
}

/**
 * @brief returns the string representation of the expression
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Expression::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Expression:", "", indent) + (value ? value->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the call
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Call::to_string(int indent) const -> std::string {
  std::string ret = fmt::format("\n{: >{}}Call:", "", indent) + name + "(";
  for (const auto& arg : args) {
    ret += arg->to_string(indent + 1) + ", ";
  }
  return ret + ")\n";
}

/**
 * @brief returns the string representation of the declaration
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Declaration::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Declaration:", "", indent) + name + " as " + type +
         (value ? value->to_string(indent + 1) : "") + '\n';
}

/**
 * @brief returns the string representation of the return
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Return::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Return:", "", indent) + (value ? value->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the exit
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Exit::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Exit:", "", indent) + (value ? value->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the print
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Print::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Print:", "", indent) + (value ? value->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the if
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::If::to_string(int indent) const -> std::string {
  std::string ret = fmt::format("\n{: >{}}If:", "", indent) + condition->to_string(indent + 1) + ":\n";
  for (const auto& statement : body) {
    ret += statement.to_string(indent + 1);
  }
  if (!elseBody.empty()) {
    ret += fmt::format("\n{: >{}}else:\n", "", indent);
    for (const auto& statement : elseBody) {
      ret += statement.to_string(indent + 1);
    }
  }
  return ret;
}

/**
 * @brief returns the string representation of the statement
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Statement::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      statement, [&](const std::unique_ptr<Assignment>& assignment) { return assignment->to_string(indent); },
      [&](const std::unique_ptr<Type>& type) { return type->to_string(indent); },
      [&](const std::unique_ptr<If>& if_) { return if_->to_string(indent); },
      [&](const std::unique_ptr<Print>& print) { return print->value->to_string(indent); },
      [&](const std::unique_ptr<Exit>& exit) { return exit->value->to_string(indent); },
      [&](const std::unique_ptr<Declaration>& declaration) { return declaration->to_string(indent); },
      [&](const std::unique_ptr<Func>& func) { return func->to_string(indent); },
      [&](const std::unique_ptr<Call>& call) { return call->to_string(indent); },
      [&](const std::unique_ptr<Return>& return_) { return return_->value->to_string(indent); },
      [&](const std::unique_ptr<While>& while_) { return while_->to_string(indent); },
      [&](std::nullptr_t) { return std::string("null\n"); });
}

/**
 * @brief returns the string representation of the while
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::While::to_string(int indent) const -> std::string {
  std::string ret = fmt::format("\n{: >{}}While:", "", indent) + condition->to_string(indent + 1) + ":\n";
  for (const auto& statement : body) {
    ret += statement.to_string(indent + 1);
  }
  return ret;
}

/**
 * @brief returns the string representation of the comparison
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Comparison::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Comparison", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the equality
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Equality::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Equality:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? (equal ? "== " : "!= ") + right->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the term
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Term::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Term:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the factor
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Factor::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Factor:", "", indent) + left->to_string(indent + 1) + " " +
         (right ? op_to_string(op) + " " + right->to_string(indent + 1) : "");
}

/**
 * @brief returns the string representation of the unary
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Unary::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      value,
      [&](const std::unique_ptr<Unary>& unary) {
        return fmt::format("\n{: >{}}Unary:", "", indent) + op_to_string(op) + unary->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Primary>& value) {
        return fmt::format("\n{: >{}}Unary:", "", indent) + (value ? value->to_string(indent + 1) : "");
      });
}

/**
 * @brief returns the string representation of the variable
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Variable::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Variable:", "", indent) + name + (attribute ? "." + attribute.value() : "");
}

/**
 * @brief returns the string representation of the function definition
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Func::to_string(int indent) const -> std::string {
  std::string ret = fmt::format("\n{: >{}}Func:", "", indent) + name + "(";
  for (const auto& arg : args) {
    ret += arg->to_string(indent + 1) + ", ";
  }
  ret += "):\n";
  for (const auto& statement : body) {
    ret += statement.to_string(indent + 1);
  }
  return ret;
}

/**
 * @brief returns the string representation of the terminal
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Terminal::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      value, [&](const Token& token) { return fmt::format("\n{: >{}}Terminal:", "", indent) + token.value; },
      [&](const std::unique_ptr<String>& str) {
        return fmt::format("\n{: >{}}Terminal:", "", indent) + str->value;
      },
      [&](const std::unique_ptr<Variable>& variable) {
        return fmt::format("\n{: >{}}Terminal:", "", indent) + variable->to_string(indent + 1);
      });
}

/**
 * @brief returns the string representation of the type
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Type::to_string(int indent) const -> std::string {
  return fmt::format("\n{: >{}}Type:", "", indent) + name;
}

/**
 * @brief returns the string representation of the primary
 * 
 * @param indent spaces to indent
 * @return std::string string representation
 */
auto AST::Primary::to_string(int indent) const -> std::string {
  return belt::overloaded_visit<std::string>(
      value,
      [&](const std::unique_ptr<Expression>& expression) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + expression->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Terminal>& terminal) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + terminal->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Call>& call) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + call->to_string(indent + 1);
      },
      [&](const std::unique_ptr<Variable>& variable) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + variable->to_string(indent + 1);
      },
      [&](const std::unique_ptr<String>& str) {
        return fmt::format("\n{: >{}}Primary:", "", indent) + str->value;
      });
}
}  // namespace kuso
