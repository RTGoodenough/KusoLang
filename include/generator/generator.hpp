#pragma once

#include <memory>

#include <belt/class_macros.hpp>
#include <belt/file.hpp>

#include "emitter/create_emitter.hpp"
#include "emitter/emitter.hpp"
#include "parser/ast.hpp"

namespace kuso {
class Generator {
  NON_DEFAULT_CONSTRUCTIBLE(Generator)
  DEFAULT_DESTRUCTIBLE(Generator)
  DEFAULT_MOVABLE(Generator)
  NON_COPYABLE(Generator)

 public:
  explicit Generator(Emitter::Type type, const std::filesystem::path& output_path)
      : _emitter(create_emitter(type, output_path)) {}

  void generate(const AST&);

 private:
  std::unique_ptr<Emitter> _emitter;
};
}  // namespace kuso