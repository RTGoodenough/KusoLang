
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
  constexpr int ITERATIONS = 5000;
  constexpr int LENGTH = 100;

  std::random_device              rnd;
  std::mt19937                    gen(rnd());
  std::uniform_int_distribution<> dis(1, LENGTH);

  kuso::Lexer lexer;

  for (int i = 0; i < ITERATIONS; i++) {
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

TEST(Lexer, Keywords) {
  std::string              input("return type print exit if else for while");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 9);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::RETURN);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::TYPE);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::PRINT);
  ASSERT_EQ(tokens[3].type, kuso::Token::Type::EXIT);
  ASSERT_EQ(tokens[4].type, kuso::Token::Type::IF);
  ASSERT_EQ(tokens[5].type, kuso::Token::Type::ELSE);
  ASSERT_EQ(tokens[6].type, kuso::Token::Type::FOR);
  ASSERT_EQ(tokens[7].type, kuso::Token::Type::WHILE);
  ASSERT_EQ(tokens[8].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, NumberToken) {
  std::string              input("12345");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::NUMBER);
  ASSERT_EQ(tokens[0].value, "12345");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, MultiCharIdentifier) {
  std::string              input("abc123");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::IDENTIFIER);
  ASSERT_EQ(tokens[0].value, "abc123");
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, ColonToken) {
  std::string              input(":");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::COLON);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, SemiColonToken) {
  std::string              input(";");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::SEMI_COLON);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, PlusToken) {
  std::string              input("+");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::PLUS);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, AsteriskToken) {
  std::string              input("*");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::ASTERISK);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, EqualToken) {
  std::string              input("=");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, NotEqualToken) {
  std::string              input("!=");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::NOT_EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, GreaterThanToken) {
  std::string              input(">");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::GREATER_THAN);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, GreaterThanOrEqualToken) {
  std::string              input(">=");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::GREATER_THAN_EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, LessThanToken) {
  std::string              input("<");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::LESS_THAN);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, LessThanOrEqualToken) {
  std::string              input("<=");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::LESS_THAN_EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, BoolEqualToken) {
  std::string              input("==");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::BOOL_EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, BoolEqualTokenWithSpaces) {
  std::string              input("= =");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 3);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::EQUAL);
  ASSERT_EQ(tokens[2].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, DollarToken) {
  std::string              input("$");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::DOLLAR);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, ExlamationToken) {
  std::string              input("!");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::EXCLAMATION);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, AmpersandToken) {
  std::string              input("&");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::AMPERSAND);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, PipeToken) {
  std::string              input("|");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::PIPE);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, CaretToken) {
  std::string              input("^");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::CARET);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}

TEST(Lexer, ArrowToken) {
  std::string              input("->");
  kuso::Lexer              lexer;
  std::vector<kuso::Token> tokens = lexer.tokenize(input);
  ASSERT_EQ(tokens.size(), 2);
  ASSERT_EQ(tokens[0].type, kuso::Token::Type::ARROW);
  ASSERT_EQ(tokens[1].type, kuso::Token::Type::END_OF_FILE);
}