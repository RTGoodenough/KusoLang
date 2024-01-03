ANTLR_FLAGS = -Werror -Dlanguage=Cpp -long-messages
GENERATED_DIR = ./generated

grammar: $(wildcard *.g4)
	antlr4 $(ANTLR_FLAGS) Kuso.g4 -o $(GENERATED_DIR)