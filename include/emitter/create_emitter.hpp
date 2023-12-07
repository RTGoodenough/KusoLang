#pragma once

#include "emitter.hpp"
#include "emitter_asm.hpp"

namespace kuso {
[[nodiscard]] auto create_emitter(Emitter::Type, const std::filesystem::path&) -> std::unique_ptr<Emitter>;

[[nodiscard]] auto to_emitter_type(const std::string&) -> Emitter::Type;
}  // namespace kuso
