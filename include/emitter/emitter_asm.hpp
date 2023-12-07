#pragma once

#include <belt/file.hpp>

#include "emitter/emitter.hpp"

namespace kuso {
class Emitter_ASM : public Emitter {
  NON_DEFAULT_CONSTRUCTIBLE(Emitter_ASM)
  DEFAULT_DESTRUCTIBLE(Emitter_ASM)
  DEFAULT_MOVABLE(Emitter_ASM)
  NON_COPYABLE(Emitter_ASM)

 public:
  explicit Emitter_ASM(const std::filesystem::path& filepath) : _output(filepath) {}

  void emit_setup() override;

  void finish() override;

 private:
  belt::File _output;
};
}  // namespace kuso