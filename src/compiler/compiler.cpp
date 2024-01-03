
#include "compiler/compiler.hpp"

#include <filesystem>

#include <KusoLexer.h>
#include <KusoParser.h>

#include <spdlog/spdlog.h>

namespace kuso {
void Compiler::compile(const std::filesystem::path& filepath) {
  if (!std::filesystem::exists(filepath)) {
    CompileException::error("Source File Does NOT Exist at " + filepath.string());
  }

  std::ifstream               srcFile(filepath);
  antlr4::ANTLRInputStream    input(srcFile);
  KusoLexer                   lexer(&input);
  antlr4::CommonTokenStream   tokens(&lexer);
  KusoParser                  parser(&tokens);
  KusoParser::ProgramContext* tree = parser.program();
}
}  // namespace kuso