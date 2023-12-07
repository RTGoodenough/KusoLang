/**
 * @file arg_types.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-08
 * 
 * @copyright Copyright 2023 Rolland Goodenough
 * 
 * This file is part of Pirate which is released under the MIT License
 * See file LICENSE for the full License
*/

#ifndef PIRATE_ARG_TYPES_HPP
#define PIRATE_ARG_TYPES_HPP

#include <cstdint>
#include <type_traits>

namespace pirate {
enum class ArgType : uint8_t { OPTIONAL = 0x00, REQUIRED = 0x01, VALUE_REQUIRED = 0x02 };

inline constexpr auto operator|(ArgType Lhs, ArgType Rhs) -> ArgType {
  return static_cast<ArgType>(static_cast<std::underlying_type_t<ArgType>>(Lhs) |
                              static_cast<std::underlying_type_t<ArgType>>(Rhs));
}

inline constexpr auto operator&(ArgType Lhs, ArgType Rhs) -> ArgType {
  return static_cast<ArgType>(static_cast<std::underlying_type_t<ArgType>>(Lhs) &
                              static_cast<std::underlying_type_t<ArgType>>(Rhs));
}

inline constexpr auto operator==(ArgType Lhs, ArgType Rhs) -> bool {
  return static_cast<std::underlying_type_t<ArgType>>(Lhs) ==
         static_cast<std::underlying_type_t<ArgType>>(Rhs);
}

inline constexpr auto operator!=(ArgType Lhs, ArgType Rhs) -> bool {
  return static_cast<std::underlying_type_t<ArgType>>(Lhs) !=
         static_cast<std::underlying_type_t<ArgType>>(Rhs);
}
}  // namespace pirate

#endif
