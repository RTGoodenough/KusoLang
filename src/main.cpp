
#include "parser/parser.hpp"

auto main() -> int {
  kuso::Parser parser("test.kuso");
  auto         ast = parser.parse();

  return 0;
}