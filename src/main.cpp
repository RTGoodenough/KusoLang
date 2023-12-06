
#include "parser/parser.hpp"

auto main() -> int {
  kuso::Parser parser("temp/test.kuso");
  auto         ast = parser.parse();
  std::cout << ast.to_string();
  return 0;
}