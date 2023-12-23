

#include <gtest/gtest.h>

#include "logging/logging.hpp"

auto main() -> int {
  kuso::Logging::set_level(kuso::Logging::Level::NONE);

  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}