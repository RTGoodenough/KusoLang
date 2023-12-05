
#include "lexer/lexer.hpp"

auto main() -> int {
  auto lexer = kuso::Lexer("temp/test.kuso");
  for (const auto& token : lexer.by_token()) {
    print_token(token);
  }
}