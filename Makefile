TARGET ?= target
CFG ?= debug

CC := clang++
CFLAGS := -Wall -Wextra

ifeq ($(CFG),release)
CFLAGS += -O3
endif

OUT := $(TARGET)/$(CFG)
$(shell mkdir -p $(OUT))

MAIN := $(TARGET)/main

SOURCE_MAIN := src/main.cc
# Main mustn't have a corresponding header.
SOURCES := $(patsubst %.hh,%.cc,$(wildcard src/*.hh))

OBJECTS := $(patsubst src/%.cc,$(OUT)/%.o,$(SOURCES))

.PHONY: run
run: $(MAIN)
	$(RUN) ./$<

$(MAIN): $(OBJECTS) $(SOURCE_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS): $(OUT)/%.o: src/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(TARGET)
