CC := gcc

# ───────────────────────────────────────────────────────
# Terminal Colors
# ───────────────────────────────────────────────────────

GREEN  := \033[0;32m
BLUE   := \033[0;34m
YELLOW := \033[1;33m
RED    := \033[0;31m
CYAN   := \033[0;36m
RESET  := \033[0m

# ───────────────────────────────────────────────────────
# Verbose Build Control
# ───────────────────────────────────────────────────────

VERBOSE ?= 1

ifeq ($(VERBOSE),1)
    ECHO = echo
else
    ECHO = true
endif

# ───────────────────────────────────────────────────────
# Compiler Options
# ───────────────────────────────────────────────────────

WARN ?= 0

LOUD ?= 0

ifeq ($(WARN),1)
    WARNFLAGS := -Wall -Wextra
else
    WARNFLAGS :=
endif

ifeq ($(LOUD),1)
    WARNFLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror
else
    WARNFLAGS :=
endif

CFLAGS := $(WARNFLAGS) -g \
          -Isrc/shared \
          -Ilibs/logging/include \
          -Ilibs/objects/include \
		  -Ilibs/gcurses/include \
          -MMD -MP
# -std=c11

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)

BUILD_DIR := build
BIN_DIR   := bin

# ───────────────────────────────────────────────────────
# Source Files
# ───────────────────────────────────────────────────────

SHARED_SRCS := $(shell find src/shared -name "*.c")

GAME_SRCS := $(shell find src/game -name "*.c")
EDITOR_SRCS := $(shell find src/editor -name "*.c")
DEVSTUDIO_SRCS := $(shell find src/devstudio -name "*.c")

# ───────────────────────────────────────────────────────
# Objects
# ───────────────────────────────────────────────────────

SHARED_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SHARED_SRCS))

GAME_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(GAME_SRCS))
EDITOR_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(EDITOR_SRCS))
DEVSTUDIO_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(DEVSTUDIO_SRCS))

ALL_OBJS := \
	$(SHARED_OBJS) \
	$(GAME_OBJS) \
	$(EDITOR_OBJS) \
	$(DEVSTUDIO_OBJS)

DEPS := $(ALL_OBJS:.o=.d)

LIB_DIRS := \
	-Llibs/logging/lib \
	-Llibs/objects/lib \
	-Llibs/gcurses/lib 

LIBS := \
	-llogging \
	-lgcurses \
	-lobjects 
	

LOGGING_LIB := libs/logging/lib/liblogging.a
OBJECTS_LIB := libs/objects/lib/libobjects.a
GCURSES_LIB := libs/gcurses/lib/libgcurses.a

# ───────────────────────────────────────────────────────
# Default Target
# ───────────────────────────────────────────────────────

.PHONY: all libraries

all: game editor devstudio

# ───────────────────────────────────────────────────────
# Static Libraries
# ───────────────────────────────────────────────────────

libraries: objects logging gcurses

objects:
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building Objects Library             │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@$(MAKE) -C libs/objects

logging: objects
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building Logging Library             │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@$(MAKE) -C libs/logging

gcurses: objects
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building gcurses Library             │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@$(MAKE) -C libs/gcurses


# ───────────────────────────────────────────────────────
# Executables
# ───────────────────────────────────────────────────────

game: $(LOGGING_LIB) $(OBJECTS_LIB) $(GAME_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building Game                        │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@mkdir -p $(BIN_DIR)
	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/game.grg"
	@$(CC) \
		$(GAME_OBJS) \
		$(SHARED_OBJS) \
		$(LIB_DIRS) \
		$(LIBS) \
		$(SDL_LIBS) \
		-o $(BIN_DIR)/game.grg

editor: $(LOGGING_LIB) $(OBJECTS_LIB) $(EDITOR_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building Editor                      │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@mkdir -p $(BIN_DIR)
	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/editor.grg"
	@$(CC) \
		$(EDITOR_OBJS) \
		$(SHARED_OBJS) \
		$(LIB_DIRS) \
		$(LIBS) \
		$(SDL_LIBS) \
		-o $(BIN_DIR)/editor.grg

devstudio: $(LOGGING_LIB) $(OBJECTS_LIB) $(DEVSTUDIO_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)┌──────────────────────────────────────┐$(RESET)"
	@$(ECHO) "$(CYAN)│ Building DevStudio                   │$(RESET)"
	@$(ECHO) "$(CYAN)└──────────────────────────────────────┘$(RESET)"
	@mkdir -p $(BIN_DIR)

	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/launcher-wrapper"
	@$(CC) \
		$(BUILD_DIR)/devstudio/launcher-wrapper.o \
		-o $(BIN_DIR)/launcher-wrapper

	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/devstudio.grg"
	@$(CC) \
		$(filter-out $(BUILD_DIR)/devstudio/launcher-wrapper.o,$(DEVSTUDIO_OBJS)) \
		$(SHARED_OBJS) \
		$(LIB_DIRS) \
		$(LIBS) \
		-o $(BIN_DIR)/devstudio.grg

# ───────────────────────────────────────────────────────
# Compilation
# ───────────────────────────────────────────────────────

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(ECHO) "$(BLUE)[CC]$(RESET) $<"
	@$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

-include $(DEPS)

# ───────────────────────────────────────────────────────
# Utility Targets
# ───────────────────────────────────────────────────────

.PHONY: clean rebuild

clean:
	@$(ECHO) "$(RED)[CLEAN]$(RESET) Removing build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@$(ECHO) "$(GREEN)Clean complete.$(RESET)"

rebuild: clean all