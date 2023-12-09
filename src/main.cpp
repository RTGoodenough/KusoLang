
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

  kuso::Parser    parser(inpath);
  kuso::Generator generator(outpath);

  auto ast = parser.parse();
  generator.generate(ast);

  return 0;
}