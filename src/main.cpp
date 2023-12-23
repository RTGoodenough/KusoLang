/**
 * @file main.cpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#include "generator/generator.hpp"
#include "logging/logging.hpp"
#include "parser/parser.hpp"
#include "setup/setup.hpp"
#include "types/arg_types.hpp"

auto main(int argc, const char** argv) -> int {
  if (!kuso::initialize(argc, argv)) return 0;

  std::filesystem::path inpath = pirate::Args::get("in");
  std::filesystem::path outpath = pirate::Args::get("out");

  kuso::Logging::set_level(kuso::Logging::level(pirate::Args::get("log")));

  kuso::Parser parser;
  auto         ast = parser.parse(inpath);

  if (ast) {
    kuso::Generator generator(outpath);
    kuso::Logging::debug(ast->to_string());
    generator.generate(ast.value());
    return 0;
  }

  return -1;
}