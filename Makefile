CFLAGS=-Wall -O2 -Weffc++ -pedantic   -pedantic-errors -Wextra -Wcast-align		\
-Wcast-qual -Wconversion  -Wdisabled-optimization  -Wfloat-equal -Wformat=2		\
-Wformat-nonliteral -Wformat-security   -Wformat-y2k  -Wimport  -Winit-self		\
-Winline  -Winvalid-pch    -Wmissing-field-initializers							\
-Wmissing-format-attribute    -Wmissing-include-dirs -Wmissing-noreturn			\
-Wpacked  -Wpointer-arith  -Wredundant-decls  -Wshadow -Wstack-protector		\
-Wstrict-aliasing=2 -Wswitch-default  -Wswitch-enum  -Wunreachable-code -Wunused\
-Wunused-but-set-variable  -Wunused-parameter  -Wvariadic-macros				\
-Wwrite-strings

ifdef ASan
	CLFLAGS+=-fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
endif

CC=g++

BINDIR=./bin

DEPS=./src/*.hpp ./src/*.cpp

TEST_GEN=./tests/generater.cpp

fastmap: $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) -o ./bin/fastmap.out

tests: $(TEST_GEN)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(TEST_GEN) -o ./bin/generator.out
	./bin/generator.out > ./tests/tests.txt

clean:
	rm ./bin/*
	rmdir ./bin
