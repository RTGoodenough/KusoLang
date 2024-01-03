
// Generated from Kuso.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "KusoParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by KusoParser.
 */
class  KusoListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(KusoParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(KusoParser::ProgramContext *ctx) = 0;

  virtual void enterStatement(KusoParser::StatementContext *ctx) = 0;
  virtual void exitStatement(KusoParser::StatementContext *ctx) = 0;

  virtual void enterAssignment(KusoParser::AssignmentContext *ctx) = 0;
  virtual void exitAssignment(KusoParser::AssignmentContext *ctx) = 0;

  virtual void enterExpression(KusoParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(KusoParser::ExpressionContext *ctx) = 0;


};

