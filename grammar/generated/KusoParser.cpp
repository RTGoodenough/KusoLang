
// Generated from Kuso.g4 by ANTLR 4.13.1


#include "KusoListener.h"

#include "KusoParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct KusoParserStaticData final {
  KusoParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  KusoParserStaticData(const KusoParserStaticData&) = delete;
  KusoParserStaticData(KusoParserStaticData&&) = delete;
  KusoParserStaticData& operator=(const KusoParserStaticData&) = delete;
  KusoParserStaticData& operator=(KusoParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag kusoParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
KusoParserStaticData *kusoParserStaticData = nullptr;

void kusoParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (kusoParserStaticData != nullptr) {
    return;
  }
#else
  assert(kusoParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<KusoParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "assignment", "expression"
    },
    std::vector<std::string>{
      "", "'->'", "'('", "')'", "", "", "", "", "", "'-'"
    },
    std::vector<std::string>{
      "", "", "", "", "WS", "ID", "INT", "FLOAT", "BINOP", "UNOP"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,9,42,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,1,0,4,0,10,8,0,11,0,12,0,11,
  	1,1,1,1,3,1,16,8,1,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,
  	3,1,3,3,3,32,8,3,1,3,1,3,1,3,5,3,37,8,3,10,3,12,3,40,9,3,1,3,0,1,6,4,
  	0,2,4,6,0,0,44,0,9,1,0,0,0,2,15,1,0,0,0,4,17,1,0,0,0,6,31,1,0,0,0,8,10,
  	3,2,1,0,9,8,1,0,0,0,10,11,1,0,0,0,11,9,1,0,0,0,11,12,1,0,0,0,12,1,1,0,
  	0,0,13,16,3,4,2,0,14,16,3,6,3,0,15,13,1,0,0,0,15,14,1,0,0,0,16,3,1,0,
  	0,0,17,18,3,6,3,0,18,19,5,1,0,0,19,20,5,5,0,0,20,5,1,0,0,0,21,22,6,3,
  	-1,0,22,32,5,5,0,0,23,32,5,6,0,0,24,32,5,7,0,0,25,26,5,2,0,0,26,27,3,
  	6,3,0,27,28,5,3,0,0,28,32,1,0,0,0,29,30,5,9,0,0,30,32,3,6,3,1,31,21,1,
  	0,0,0,31,23,1,0,0,0,31,24,1,0,0,0,31,25,1,0,0,0,31,29,1,0,0,0,32,38,1,
  	0,0,0,33,34,10,3,0,0,34,35,5,8,0,0,35,37,3,6,3,4,36,33,1,0,0,0,37,40,
  	1,0,0,0,38,36,1,0,0,0,38,39,1,0,0,0,39,7,1,0,0,0,40,38,1,0,0,0,4,11,15,
  	31,38
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  kusoParserStaticData = staticData.release();
}

}

KusoParser::KusoParser(TokenStream *input) : KusoParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

KusoParser::KusoParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  KusoParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *kusoParserStaticData->atn, kusoParserStaticData->decisionToDFA, kusoParserStaticData->sharedContextCache, options);
}

KusoParser::~KusoParser() {
  delete _interpreter;
}

const atn::ATN& KusoParser::getATN() const {
  return *kusoParserStaticData->atn;
}

std::string KusoParser::getGrammarFileName() const {
  return "Kuso.g4";
}

const std::vector<std::string>& KusoParser::getRuleNames() const {
  return kusoParserStaticData->ruleNames;
}

const dfa::Vocabulary& KusoParser::getVocabulary() const {
  return kusoParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView KusoParser::getSerializedATN() const {
  return kusoParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

KusoParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KusoParser::StatementContext *> KusoParser::ProgramContext::statement() {
  return getRuleContexts<KusoParser::StatementContext>();
}

KusoParser::StatementContext* KusoParser::ProgramContext::statement(size_t i) {
  return getRuleContext<KusoParser::StatementContext>(i);
}


size_t KusoParser::ProgramContext::getRuleIndex() const {
  return KusoParser::RuleProgram;
}

void KusoParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void KusoParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

KusoParser::ProgramContext* KusoParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, KusoParser::RuleProgram);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(9); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(8);
      statement();
      setState(11); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 740) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

KusoParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KusoParser::AssignmentContext* KusoParser::StatementContext::assignment() {
  return getRuleContext<KusoParser::AssignmentContext>(0);
}

KusoParser::ExpressionContext* KusoParser::StatementContext::expression() {
  return getRuleContext<KusoParser::ExpressionContext>(0);
}


size_t KusoParser::StatementContext::getRuleIndex() const {
  return KusoParser::RuleStatement;
}

void KusoParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void KusoParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

KusoParser::StatementContext* KusoParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, KusoParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(15);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(13);
      assignment();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(14);
      expression(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignmentContext ------------------------------------------------------------------

KusoParser::AssignmentContext::AssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KusoParser::ExpressionContext* KusoParser::AssignmentContext::expression() {
  return getRuleContext<KusoParser::ExpressionContext>(0);
}

tree::TerminalNode* KusoParser::AssignmentContext::ID() {
  return getToken(KusoParser::ID, 0);
}


size_t KusoParser::AssignmentContext::getRuleIndex() const {
  return KusoParser::RuleAssignment;
}

void KusoParser::AssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignment(this);
}

void KusoParser::AssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignment(this);
}

KusoParser::AssignmentContext* KusoParser::assignment() {
  AssignmentContext *_localctx = _tracker.createInstance<AssignmentContext>(_ctx, getState());
  enterRule(_localctx, 4, KusoParser::RuleAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(17);
    expression(0);
    setState(18);
    match(KusoParser::T__0);
    setState(19);
    match(KusoParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

KusoParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KusoParser::ExpressionContext::ID() {
  return getToken(KusoParser::ID, 0);
}

tree::TerminalNode* KusoParser::ExpressionContext::INT() {
  return getToken(KusoParser::INT, 0);
}

tree::TerminalNode* KusoParser::ExpressionContext::FLOAT() {
  return getToken(KusoParser::FLOAT, 0);
}

std::vector<KusoParser::ExpressionContext *> KusoParser::ExpressionContext::expression() {
  return getRuleContexts<KusoParser::ExpressionContext>();
}

KusoParser::ExpressionContext* KusoParser::ExpressionContext::expression(size_t i) {
  return getRuleContext<KusoParser::ExpressionContext>(i);
}

tree::TerminalNode* KusoParser::ExpressionContext::UNOP() {
  return getToken(KusoParser::UNOP, 0);
}

tree::TerminalNode* KusoParser::ExpressionContext::BINOP() {
  return getToken(KusoParser::BINOP, 0);
}


size_t KusoParser::ExpressionContext::getRuleIndex() const {
  return KusoParser::RuleExpression;
}

void KusoParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void KusoParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<KusoListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}


KusoParser::ExpressionContext* KusoParser::expression() {
   return expression(0);
}

KusoParser::ExpressionContext* KusoParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  KusoParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  KusoParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 6;
  enterRecursionRule(_localctx, 6, KusoParser::RuleExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(31);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KusoParser::ID: {
        setState(22);
        match(KusoParser::ID);
        break;
      }

      case KusoParser::INT: {
        setState(23);
        match(KusoParser::INT);
        break;
      }

      case KusoParser::FLOAT: {
        setState(24);
        match(KusoParser::FLOAT);
        break;
      }

      case KusoParser::T__1: {
        setState(25);
        match(KusoParser::T__1);
        setState(26);
        expression(0);
        setState(27);
        match(KusoParser::T__2);
        break;
      }

      case KusoParser::UNOP: {
        setState(29);
        match(KusoParser::UNOP);
        setState(30);
        expression(1);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(38);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression);
        setState(33);

        if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
        setState(34);
        match(KusoParser::BINOP);
        setState(35);
        expression(4); 
      }
      setState(40);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

bool KusoParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 3: return expressionSempred(antlrcpp::downCast<ExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool KusoParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

void KusoParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  kusoParserInitialize();
#else
  ::antlr4::internal::call_once(kusoParserOnceFlag, kusoParserInitialize);
#endif
}
