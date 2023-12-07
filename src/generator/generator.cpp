

#include "generator/generator.hpp"

namespace kuso {
void Generator::generate(const AST&) {
  _emitter->emit_setup();
  _emitter->finish();
}
}  // namespace kuso