
// Generated from Kuso.g4 by ANTLR 4.13.1


#include "KusoLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct KusoLexerStaticData final {
  KusoLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  KusoLexerStaticData(const KusoLexerStaticData&) = delete;
  KusoLexerStaticData(KusoLexerStaticData&&) = delete;
  KusoLexerStaticData& operator=(const KusoLexerStaticData&) = delete;
  KusoLexerStaticData& operator=(KusoLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag kusolexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
KusoLexerStaticData *kusolexerLexerStaticData = nullptr;

void kusolexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (kusolexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(kusolexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<KusoLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "WS", "ID", "INT", "FLOAT", "BINOP", "UNOP"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'->'", "'('", "')'", "", "", "", "", "", "'-'"
    },
    std::vector<std::string>{
      "", "", "", "", "WS", "ID", "INT", "FLOAT", "BINOP", "UNOP"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,9,66,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,
  	2,7,7,7,2,8,7,8,1,0,1,0,1,0,1,1,1,1,1,2,1,2,1,3,4,3,28,8,3,11,3,12,3,
  	29,1,3,1,3,1,4,4,4,35,8,4,11,4,12,4,36,1,5,3,5,40,8,5,1,5,4,5,43,8,5,
  	11,5,12,5,44,1,6,3,6,48,8,6,1,6,4,6,51,8,6,11,6,12,6,52,1,6,1,6,4,6,57,
  	8,6,11,6,12,6,58,1,6,1,6,1,7,1,7,1,8,1,8,0,0,9,1,1,3,2,5,3,7,4,9,5,11,
  	6,13,7,15,8,17,9,1,0,4,3,0,9,10,13,13,32,32,2,0,65,90,97,122,1,0,48,57,
  	3,0,42,43,45,45,47,47,72,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,
  	0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,1,
  	19,1,0,0,0,3,22,1,0,0,0,5,24,1,0,0,0,7,27,1,0,0,0,9,34,1,0,0,0,11,39,
  	1,0,0,0,13,47,1,0,0,0,15,62,1,0,0,0,17,64,1,0,0,0,19,20,5,45,0,0,20,21,
  	5,62,0,0,21,2,1,0,0,0,22,23,5,40,0,0,23,4,1,0,0,0,24,25,5,41,0,0,25,6,
  	1,0,0,0,26,28,7,0,0,0,27,26,1,0,0,0,28,29,1,0,0,0,29,27,1,0,0,0,29,30,
  	1,0,0,0,30,31,1,0,0,0,31,32,6,3,0,0,32,8,1,0,0,0,33,35,7,1,0,0,34,33,
  	1,0,0,0,35,36,1,0,0,0,36,34,1,0,0,0,36,37,1,0,0,0,37,10,1,0,0,0,38,40,
  	5,45,0,0,39,38,1,0,0,0,39,40,1,0,0,0,40,42,1,0,0,0,41,43,7,2,0,0,42,41,
  	1,0,0,0,43,44,1,0,0,0,44,42,1,0,0,0,44,45,1,0,0,0,45,12,1,0,0,0,46,48,
  	5,45,0,0,47,46,1,0,0,0,47,48,1,0,0,0,48,50,1,0,0,0,49,51,7,2,0,0,50,49,
  	1,0,0,0,51,52,1,0,0,0,52,50,1,0,0,0,52,53,1,0,0,0,53,54,1,0,0,0,54,56,
  	5,46,0,0,55,57,7,2,0,0,56,55,1,0,0,0,57,58,1,0,0,0,58,56,1,0,0,0,58,59,
  	1,0,0,0,59,60,1,0,0,0,60,61,5,70,0,0,61,14,1,0,0,0,62,63,7,3,0,0,63,16,
  	1,0,0,0,64,65,5,45,0,0,65,18,1,0,0,0,8,0,29,36,39,44,47,52,58,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  kusolexerLexerStaticData = staticData.release();
}

}

KusoLexer::KusoLexer(CharStream *input) : Lexer(input) {
  KusoLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *kusolexerLexerStaticData->atn, kusolexerLexerStaticData->decisionToDFA, kusolexerLexerStaticData->sharedContextCache);
}

KusoLexer::~KusoLexer() {
  delete _interpreter;
}

std::string KusoLexer::getGrammarFileName() const {
  return "Kuso.g4";
}

const std::vector<std::string>& KusoLexer::getRuleNames() const {
  return kusolexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& KusoLexer::getChannelNames() const {
  return kusolexerLexerStaticData->channelNames;
}

const std::vector<std::string>& KusoLexer::getModeNames() const {
  return kusolexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& KusoLexer::getVocabulary() const {
  return kusolexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView KusoLexer::getSerializedATN() const {
  return kusolexerLexerStaticData->serializedATN;
}

const atn::ATN& KusoLexer::getATN() const {
  return *kusolexerLexerStaticData->atn;
}




void KusoLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  kusolexerLexerInitialize();
#else
  ::antlr4::internal::call_once(kusolexerLexerOnceFlag, kusolexerLexerInitialize);
#endif
}
