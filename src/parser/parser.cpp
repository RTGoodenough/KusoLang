

#include "parser/parser.hpp"
#include "lexer/token.hpp"

namespace kuso {

auto Parser::parse() -> AST {
  for (auto [token, lookahead] : _lexer.by_token()) {
    print_token(token);
    print_token(lookahead);
  }
  return _ast;
}
}  // namespace kuso