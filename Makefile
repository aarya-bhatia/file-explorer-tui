all: main

SRC_FILES=$(shell find src -type f -name "*.cpp" -o -name "*.c")
SRC_OBJECTS=$(SRC_FILES:src/%=obj/%.o)

LDFLAGS=-lncurses -lm
CXXFLAGS=-c -Wall -std=c++14
CFLAGS=-c -Wall -std=c99 

main: $(SRC_OBJECTS)
	g++ $(LDFLAGS) $^ -o $@

obj/%.cpp.o: src/%.cpp
	mkdir -p $(dir $@)
	g++ $(CXXFLAGS) $< -o $@

obj/%.c.o: src/%.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $< -o $@

clean:
	rm -rf main obj/

