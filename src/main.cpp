
#include "parser/parser.hpp"

auto main() -> int {
  kuso::Parser parser("temp/test.kuso");
  auto         ast = parser.parse();

  return 0;
}