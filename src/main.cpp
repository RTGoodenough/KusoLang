
#include <pirate.hpp>

#include "emitter/create_emitter.hpp"
#include "generator/generator.hpp"
#include "parser/parser.hpp"
#include "setup/setup.hpp"
#include "types/arg_types.hpp"

auto main(int argc, const char** argv) -> int {
  register_args();
  pirate::Args::parse(argc, argv);

  kuso::Parser    parser(pirate::Args::get("in"));
  kuso::Generator generator(kuso::to_emitter_type(pirate::Args::get("lang")), pirate::Args::get("out"));

  auto ast = parser.parse();
  std::cout << ast.to_string();

  generator.generate(ast);
  return 0;
}