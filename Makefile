all: main

SRC_FILES=$(shell find src -type f -name "*.cpp")
SRC_OBJECTS=$(SRC_FILES:src/%=obj/%.o)

LDFLAGS=-lncurses -lm
CFLAGS=-c -Wall -std=c++14

main: $(SRC_OBJECTS)
	g++ $(LDFLAGS) $^ -o $@

obj/%.o: src/%
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $< -o $@

clean:
	rm -rf main obj/

