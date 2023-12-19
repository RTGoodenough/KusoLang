#pragma once

#include <fmt/format.h>

#include "x86.hpp"

namespace kuso::x86 {

struct Address {
  enum class Mode { DIRECT, INDIRECT, INDIRECT_DISPLACEMENT };

  Mode     mode{Mode::DIRECT};
  Register reg{Register::NONE};
  int      disp{0};

  [[nodiscard]] auto to_string() const -> std::string {
    switch (mode) {
      case Mode::DIRECT:
        return reg == Register::NONE ? fmt::format("{:x}", disp) : x86::to_string(reg);
      case Mode::INDIRECT:
        return "[" + x86::to_string(reg) + "]";
      case Mode::INDIRECT_DISPLACEMENT:
        return std::to_string(disp) + "[" + x86::to_string(reg) + "]";
    }

    throw std::runtime_error("Invalid Address");
  }

  [[nodiscard]] auto operator+(int off) const -> Address {
    auto addr = *this;
    addr.disp += off;
    return addr;
  }
};

}  // namespace kuso::x86