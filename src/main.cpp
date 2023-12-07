
#include <pirate/pirate.hpp>

#include "generator/generator.hpp"
#include "parser/parser.hpp"
#include "types/arg_types.hpp"

auto main(int argc, const char** argv) -> int {
  pirate::Args::register_arg("in", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("out", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::parse(argc, argv);

  kuso::Parser    parser(pirate::Args::get("in"));
  kuso::Generator generator(pirate::Args::get("out"));

  auto ast = parser.parse();
  std::cout << ast.to_string();

  generator.generate(ast);
  return 0;
}