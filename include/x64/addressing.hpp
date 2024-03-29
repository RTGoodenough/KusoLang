/**
 * @file addressing.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <fmt/format.h>

#include "x64.hpp"

namespace kuso::x64 {

struct Address {
  enum class Mode { DIRECT, INDIRECT, INDIRECT_DISPLACEMENT };

  Mode     mode{Mode::DIRECT};
  Register reg{Register::NONE};
  int64_t  disp{0};

  [[nodiscard]] auto to_string() const -> std::string {
    switch (mode) {
      case Mode::DIRECT:
        return reg == Register::NONE ? fmt::format("{:x}", disp) : x64::to_string(reg);
      case Mode::INDIRECT:
        return "[" + x64::to_string(reg) + "]";
      case Mode::INDIRECT_DISPLACEMENT:
        return std::to_string(disp) + "[" + x64::to_string(reg) + "]";
    }

    throw std::runtime_error("Invalid Address");
  }

  /**
 * @brief Adds an offset to the displacement of the address
 * 
 * @param offset 
 * @return Address 
 */
  [[nodiscard]] auto operator+(int offset) const -> Address {
    auto addr = *this;
    addr.disp += offset;
    return addr;
  }
};

}  // namespace kuso::x64