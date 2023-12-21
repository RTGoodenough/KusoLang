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

#include <cstdlib>
#include <filesystem>
#include <pirate.hpp>

#include "generator/generator.hpp"
#include "parser/parser.hpp"
#include "setup/setup.hpp"
#include "types/arg_types.hpp"

auto main(int argc, const char** argv) -> int {
  register_args();
  pirate::Args::parse(argc, argv);

  std::filesystem::path inpath = pirate::Args::get("in");
  std::filesystem::path outpath = pirate::Args::get("out");

  kuso::Parser    parser;
  kuso::Generator generator(outpath);

  auto ast = parser.parse(inpath);
  std::cout << ast.to_string() << '\n';

  generator.generate(ast);

  return 0;
}