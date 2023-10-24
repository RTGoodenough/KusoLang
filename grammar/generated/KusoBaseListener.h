
// Generated from Kuso.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "KusoListener.h"


/**
 * This class provides an empty implementation of KusoListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  KusoBaseListener : public KusoListener {
public:

  virtual void enterProgram(KusoParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(KusoParser::ProgramContext * /*ctx*/) override { }

  virtual void enterStatement(KusoParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(KusoParser::StatementContext * /*ctx*/) override { }

  virtual void enterAssignment(KusoParser::AssignmentContext * /*ctx*/) override { }
  virtual void exitAssignment(KusoParser::AssignmentContext * /*ctx*/) override { }

  virtual void enterExpression(KusoParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(KusoParser::ExpressionContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

