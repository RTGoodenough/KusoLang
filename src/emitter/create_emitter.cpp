

#include "emitter/create_emitter.hpp"
#include "emitter/emitter.hpp"

namespace kuso {
/**
  * @brief Returns the Emitter::Type enum value corresponding to the provided string.
  * 
  * @param type 
  * @return Emitter::Type 
  */
auto to_emitter_type(const std::string& type) -> Emitter::Type {
  if (type == "asm") {
    return Emitter::Type::ASM;
  }

  throw std::runtime_error("Unknown emitter type: " + type);
}

/**
 * @brief Create an emitter object of the provided type.
 * 
 * @param type 
 * @param filepath 
 * @return std::unique_ptr<Emitter> 
 */
auto create_emitter(Emitter::Type type, const std::filesystem::path& filepath) -> std::unique_ptr<Emitter> {
  switch (type) {
    case Emitter::Type::ASM:
      return std::make_unique<Emitter_ASM>(filepath);
  }

  throw std::runtime_error("Unknown Emitter Type");
}

}  // namespace kuso