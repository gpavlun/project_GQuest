CC := gcc
AR := ar

# ───────────────────────────────────────────────────────
# Terminal Colors
# ───────────────────────────────────────────────────────

GREEN  := \033[0;32m
BLUE   := \033[0;34m
RED    := \033[0;31m
CYAN   := \033[0;36m
RESET  := \033[0m

# ───────────────────────────────────────────────────────
# Compiler Options
# ───────────────────────────────────────────────────────

WARN ?= 0

ifeq ($(WARN),1)
    WARNFLAGS := -Wall -Wextra
else
    WARNFLAGS :=
endif

CFLAGS := $(WARNFLAGS) -g \
          -Iinclude \
          -MMD -MP

# ───────────────────────────────────────────────────────
# Directories
# ───────────────────────────────────────────────────────

SRC_DIR := src
BUILD_DIR := build
LIB_DIR := lib

TARGET := $(LIB_DIR)/liblogging.a

# ───────────────────────────────────────────────────────
# Sources
# ───────────────────────────────────────────────────────

SRCS := $(shell find $(SRC_DIR) -name "*.c")

OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

DEPS := $(OBJS:.o=.d)

# ───────────────────────────────────────────────────────
# Default Target
# ───────────────────────────────────────────────────────

.PHONY: all clean

all: $(TARGET)

# ───────────────────────────────────────────────────────
# Static Library
# ───────────────────────────────────────────────────────

$(TARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	@echo "$(CYAN)[AR]$(RESET) $@"
	@$(AR) rcs $@ $^

# ───────────────────────────────────────────────────────
# Compilation
# ───────────────────────────────────────────────────────

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[CC]$(RESET) $<"
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

# ───────────────────────────────────────────────────────
# Utility Targets
# ───────────────────────────────────────────────────────

clean:
	@echo "$(RED)[CLEAN]$(RESET) Removing library build artifacts..."
	@rm -rf $(BUILD_DIR) $(LIB_DIR)