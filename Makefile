CC ?= clang
CFLAGS ?= -Wall -Wextra

TARGET ?= target
CFG ?= debug

ifeq ($(CFG),release)
CFLAGS += -O3
endif

OUT := $(TARGET)/$(CFG)
$(shell mkdir -p $(OUT))

MAIN := $(TARGET)/main

SOURCE_MAIN := src/main.c
# Main mustn't have a corresponding header.
SOURCES := $(patsubst %.h,%.c,$(wildcard src/*.h))

OBJS := $(patsubst src/%.c,$(OUT)/%.o,$(SOURCES))

.PHONY: run
run: $(MAIN)
	$(RUN) ./$<

$(MAIN): $(OBJS) $(SOURCE_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJS): $(OUT)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(TARGET)
