TARGET ?= target
CFG ?= debug

CC := clang++
CFLAGS := -Wall -Wextra

ifeq ($(CFG),release)
CFLAGS += -O3
endif

OUT := $(TARGET)/$(CFG)
$(shell mkdir -p $(OUT))

MAIN := $(OUT)/main

SOURCE_MAIN := src/main.cc
# Main mustn't have a corresponding header.
SOURCES := $(patsubst %.hh,%.cc,$(wildcard src/*.hh))

OBJECTS := $(patsubst src/%.cc,$(OUT)/%.o,$(SOURCES))

$(MAIN): $(OBJECTS) $(SOURCE_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS): $(OUT)/%.o: src/%.cc src/%.hh
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(MAIN)
	$(RUN) ./$< $(ARGS)

.PHONY: clean
clean:
	rm -rf $(TARGET)
