

#include "emitter/emitter_asm.hpp"

namespace kuso {
void Emitter_ASM::emit_setup() { _output.write("BITS 64\nSECTION .text\nglobal main\nmain:\n"); }
void Emitter_ASM::finish() { _output.write("mov rax, 60\nmov rdi, 0\nsyscall\n"); }
}  // namespace kuso