.PHONY: all clean

all: files

SRCS := $(shell find src -type f -name "*.cpp" -o -name "*.c")

# Create object lists for both build types
SRC_OBJECTS := $(SRCS:src/%=obj/%.o)
# This pattern replacement handles both .c and .cpp extensions safely
DEBUG_OBJECTS := $(SRC_OBJECTS:.o=.debug.o)

# Include dependencies for ALL objects generated
DEPS := $(SRC_OBJECTS:.o=.d) $(DEBUG_OBJECTS:.o=.d)
-include $(DEPS)

INC_DIR := $(shell find src -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIR))

LDFLAGS := -lncurses -lm
DEBUG_FLAGS := -g -O0
SANITIZER := -fsanitize=address

COMMON_FLAGS := -Wall -Wextra -MMD -MP $(INC_FLAGS) -fdiagnostics-show-template-tree
CXXFLAGS := -std=c++17 $(COMMON_FLAGS)
CFLAGS := -std=c99 $(COMMON_FLAGS)

# --- Linker Rules ---

files: $(SRC_OBJECTS)
	g++ $(SANITIZER) $^ -o $@ $(LDFLAGS)

files-debug: $(DEBUG_OBJECTS)
	g++ $^ -o $@ $(LDFLAGS)

# --- Compilation Rules ---

# Pattern for Release C++
obj/%.cpp.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ $(CXXFLAGS) $(SANITIZER) -c $< -o $@

# Pattern for Debug C++
obj/%.cpp.debug.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Pattern for C (Added debug variant for consistency)
obj/%.c.o: src/%.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

obj/%.c.debug.o: src/%.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

clean:
	rm -f files files-debug
	rm -rf obj/
