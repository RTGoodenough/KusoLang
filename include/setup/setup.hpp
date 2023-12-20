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

#include <pirate.hpp>

/**
 * @brief Registers pirate commandline arguments
 * 
 */
inline void register_args() {
  pirate::Args::register_arg("in", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("out", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("lang", "asm", pirate::ArgType::OPTIONAL | pirate::ArgType::VALUE_REQUIRED);
}