
#include <gtest/gtest.h>
#include <filesystem>
#include <random>

#include "parser/parser.hpp"

// NOLINTNEXTLINE
const std::filesystem::path TESTS_PATH = std::filesystem::path(__FILE__).parent_path() / "parser_tests";

TEST(Parser, Parse) {
  kuso::Parser parser;
  auto         ast = parser.parse(TESTS_PATH / "test1.kuso");
  ASSERT_TRUE(ast);
  if (ast) ASSERT_EQ(ast->statements().size(), 5);

  kuso::Lexer  lexer;
  kuso::Parser parser2;
  auto         tokens = lexer.tokenize(TESTS_PATH / "test1.kuso");
  auto         ast2 = parser2.parse(tokens);
  ASSERT_TRUE(ast2);
  if (ast2) ASSERT_EQ(ast2->statements().size(), 5);
}

TEST(Parser, FuzzTest) {
  constexpr int ITERATIONS = 5000;
  constexpr int LENGTH = 100;

  std::random_device              rnd;
  std::mt19937                    gen(rnd());
  std::uniform_int_distribution<> dis(1, LENGTH);
  std::uniform_int_distribution<> typeDis(0, static_cast<int>(kuso::Token::Type::COUNT) - 1);

  kuso::Parser parser;

  for (int i = 0; i < ITERATIONS; ++i) {
    // generate random tokens
    std::vector<kuso::Token> tokens;

    for (int j = 0; j < dis(gen); ++j) {
      // gnerate random token type
      auto type = static_cast<kuso::Token::Type>(typeDis(gen));
      tokens.emplace_back(type, 0, 0, "test");
    }

    ASSERT_NO_THROW(auto ast = parser.parse(tokens));
  }
}