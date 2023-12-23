/**
 * @file setup.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <filesystem>

#include <pirate.hpp>
#include <span>

#include "logging/logging.hpp"

namespace kuso {
/**
 * @brief Registers pirate commandline arguments
 * 
 */
inline void register_args() {
  pirate::Args::register_arg("in", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("out", "./kuso.out",
                             pirate::ArgType::OPTIONAL | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("log", "info", pirate::ArgType::OPTIONAL | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("s", pirate::ArgType::OPTIONAL);
  pirate::Args::register_arg("h", pirate::ArgType::OPTIONAL);
  pirate::Args::register_arg("help", pirate::ArgType::OPTIONAL);
}

inline auto initialize(int argc, const char** argv) -> bool {
  std::span<const char*> args(argv, argc);

  register_args();
  pirate::Args::parse(argc, argv);

  if (pirate::Args::has("h") || pirate::Args::has("help")) {
    kuso::Logging::info(fmt::format(
        "Usage: {} -in=<input path> [-out=<output path>] [-s] [-log=<debug|info|warn|error>]", args[0]));
    return false;
  }

  if (pirate::Args::has("s")) {
    kuso::Logging::set_level(kuso::Logging::Level::NONE);
  }

  return true;
}
}  // namespace kuso