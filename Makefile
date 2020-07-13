
SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}

CC = clang
CFLAGS = -std=c99 -g -O0 -Wno-parentheses -Wno-switch-enum -Wno-unused-value
CFLAGS += -Wno-switch
LDFLAGS += -lm

clang = $(shell which clang 2> /dev/null)
ifeq (, $(clang))
	CC = gcc
endif

# build

OUT = pascal
ifdef SystemRoot
	OUT = pascal.exe
endif

$(OUT): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@
	@printf "\e[36mCC\e[90m %s\e[0m\n" $@

clean:
	rm -f pascal $(OBJ) $(TEST_OBJ)

.PHONY: clean
