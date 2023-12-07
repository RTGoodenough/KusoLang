#pragma once

#include <belt/class_macros.hpp>
#include <belt/file.hpp>
#include <memory>
#include "generator/emitter.hpp"
#include "parser/ast.hpp"
namespace kuso {
class Generator {
  DEFAULT_DESTRUCTIBLE(Generator)
  DEFAULT_MOVABLE(Generator)

  NON_DEFAULT_CONSTRUCTIBLE(Generator)
  NON_COPYABLE(Generator)

 public:
  explicit Generator(const std::filesystem::path& output_path) : _output(output_path, std::ios_base::out) {}

  void generate(const AST&);

 private:
  belt::File               _output;
  std::unique_ptr<Emitter> _emitter;

  void output_setup();
};
}  // namespace kuso