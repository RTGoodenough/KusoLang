

#include "generator/generator.hpp"

namespace kuso {
void Generator::generate(const AST&) {
  output_setup();
  _output.write("mov rax, 60\n");
  _output.write("mov rdi, 2\n");
  _output.write("syscall\n");
}

void Generator::output_setup() { _output.write("BITS 64\nSECTION .text\nglobal main\nmain:\n"); }

}  // namespace kuso