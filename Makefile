all: files

SRC_FILES=$(shell find src -type f -name "*.cpp" -o -name "*.c")
SRC_OBJECTS=$(SRC_FILES:src/%=obj/%.o)

SRCS=$(shell find src -type f -name "*.cpp" -o -name "*.c")
DEPS = $(SRCS:src/%=obj/%.d)
-include $(DEPS)

INC_DIR=$(shell find src -type d)
INC_FLAGS=$(addprefix -I, $(INC_DIR))

LDFLAGS=-lncurses -lm -fsanitize=address
COMMON_FLAGS=-c -Wall -Wextra -fsanitize=address -MMD -MP $(INC_FLAGS) -fdiagnostics-show-template-tree
CXXFLAGS=-std=c++17 $(COMMON_FLAGS)
CFLAGS=-std=c99 $(COMMON_FLAGS)

files: $(SRC_OBJECTS)
	g++ $(LDFLAGS) $^ -o $@

obj/%.cpp.o: src/%.cpp
	mkdir -p $(dir $@)
	g++ $(CXXFLAGS) $< -o $@

obj/%.c.o: src/%.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $< -o $@

clean:
	rm -rf files obj/

