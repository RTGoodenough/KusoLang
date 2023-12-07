#pragma once

#include <pirate.hpp>

inline void register_args() {
  pirate::Args::register_arg("in", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("out", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("lang", "asm", pirate::ArgType::OPTIONAL | pirate::ArgType::VALUE_REQUIRED);
}