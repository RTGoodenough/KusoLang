

#include "emitter/emitter_x86.hpp"

namespace kuso {
void Emitter_x86::emit_setup() { _output.write("global    _start\nsection   .text\n_start:\n"); }
void Emitter_x86::finish() { _output.write("mov rax, 60\nmov rdi, 0\nsyscall\n"); }
}  // namespace kuso