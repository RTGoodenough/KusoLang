
#include <random>

#include <gtest/gtest.h>

#include "lexer/lexer.hpp"

TEST(Lexer, EmptyFile) {
  std::string              input;
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 1);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, FuzzTest) {
  std::random_device              rnd;
  std::mt19937                    gen(rnd());
  std::uniform_int_distribution<> dis(1, 100);

  kuso::Lexer lexer;

  for (int i = 0; i < 5000; i++) {
    std::string input;
    int         length = dis(gen);

    for (int j = 0; j < length; j++) {
      input += static_cast<char>(dis(gen));
    }

    ASSERT_NO_THROW(std::vector<kuso::Token> tokens = lexer.tokenize(input));
  }
}

TEST(Lexer, SingleIdentifier) {
  std::string              input("x");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "x");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultipleIdentifiers) {
  std::string              input("x y z");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 4);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "x");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[1].value, "y");
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "z");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, AssignmentStatement) {
  std::string              input("x = 10;");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 5);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "x");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[2].value, "10");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, ArithmeticExpression) {
  std::string              input("x = 5 + 3 * 2;");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 9);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "x");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[2].value, "5");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::PLUS);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "3");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::ASTERISK);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[6].value, "2");
  ASSERT_EQ(tokens[7].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[8].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SingleLineComment) {
  std::string              input("// This is a comment");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 1);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultiLineComment) {
  std::string              input("/* This is a comment\nThis is the second line */");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 1);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SingleLineCommentWithNewLine) {
  std::string              input("// This is a comment\n");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 1);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultiLineCommentWithNewLine) {
  std::string              input("/* This is a comment\nThis is the second line */\n");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 1);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SingleLineCommentWithNewLineAndCode) {
  std::string              input("// This is a comment\na : int = 0;");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 7);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "a");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "int");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "0");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultiLineCommentWithNewLineAndCode) {
  std::string              input("/* This is a comment\nThis is the second line */\na : int = 0;");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 7);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "a");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "int");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "0");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SingleLineCommentWithNewLineAndCodeAndNewLine) {
  std::string              input("// This is a comment\na : int = 0;\n");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 7);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "a");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "int");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "0");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultiLineCommentWithNewLineAndCodeAndNewLine) {
  std::string              input("/* This is a comment\nThis is the second line */\na : int = 0;\n");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 7);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "a");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "int");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "0");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SingleLineCommentWithNewLineAndCodeAndNewLineAndComment) {
  std::string              input("// This is a comment\na : int = 0;\n// This is a comment");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 7);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "a");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[2].value, "int");
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[4].value, "0");
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::END_OF_FILE);
}