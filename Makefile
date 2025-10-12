CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -I./src -I./src/tokenizer -I./src/lexer -I./src/parser -I./src/runtime

SRCS = \
	src/main.c \
	src/tokenizer/tokenizer.c \
	src/lexer/lexer.c \
	src/parser/parser.c \
	src/runtime/runtime.c

OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

all: jail

jail: $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/jail $(OBJS)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f bin/jail $(OBJS)

.PHONY: all clean
