#ifndef KUSO_LANG_COMPILER_HPP
#define KUSO_LANG_COMPILER_HPP

#include <filesystem>

#include "compiler/compiler_exception.hpp"
#include "util/class_types.hpp"

namespace kuso {
class Compiler {
  DEFAULT_CONSTRUCTIBLE(Compiler)
  DEFAULT_DESTRUCTIBLE(Compiler)
  COPYABLE(Compiler)
  MOVABLE(Compiler)

 public:
  static void compile(const std::filesystem::path& filepath);

 private:
};
}  // namespace kuso

#endif
