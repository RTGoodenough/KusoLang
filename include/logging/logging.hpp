/**
 * @file logging.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-29
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <fmt/format.h>
#include <belt/class_macros.hpp>
#include "fmt/core.h"

namespace kuso {
class Logging {
  SINGLETON(Logging)
 public:
  enum class Level { DEBUG, INFO, WARN, ERROR, NONE };

  static auto level(const std::string& lvlStr) {
    if (lvlStr == "debug") {
      return Level::DEBUG;
    }
    if (lvlStr == "info") {
      return Level::INFO;
    }
    if (lvlStr == "warn") {
      return Level::WARN;
    }
    if (lvlStr == "error") {
      return Level::ERROR;
    }
    if (lvlStr == "none") {
      return Level::NONE;
    }

    return Level::INFO;
  }

  static void set_level(Level lvl) { _level = lvl; }

  static void debug(const std::string& message) {
    if (_level <= Level::DEBUG) {
      fmt::print("[DEBUG] {}\n", message);
    }
  }

  static void info(const std::string& message) {
    if (_level <= Level::INFO) {
      fmt::print("[INFO] {}\n", message);
    }
  }

  static void warn(const std::string& message) {
    if (_level <= Level::WARN) {
      fmt::print("[WARN] {}\n", message);
    }
  }

  static void error(const std::string& message) {
    if (_level <= Level::ERROR) {
      fmt::print(stderr, "[ERROR] {}\n", message);
    }
  }

 private:
  static Level _level;
};
}  // namespace kuso