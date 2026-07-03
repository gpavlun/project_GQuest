CC := gcc

# -------------------------------------------------------
# Terminal Colors
# -------------------------------------------------------

GREEN  := \033[0;32m
BLUE   := \033[0;34m
YELLOW := \033[1;33m
RED    := \033[0;31m
CYAN   := \033[0;36m
RESET  := \033[0m

# -------------------------------------------------------
# Verbose Build Control (ECHO system)
# -------------------------------------------------------

VERBOSE ?= 1

ifeq ($(VERBOSE),1)
    ECHO = echo
else
    ECHO = true
endif

# -------------------------------------------------------
# Compiler Options
# -------------------------------------------------------

WARN ?= 0

ifeq ($(WARN),1)
    WARNFLAGS := -Wall -Wextra
else
    WARNFLAGS :=
endif

CFLAGS := $(WARNFLAGS) -g \
          -Iheaders \
          -MMD -MP
# -std=c11

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)

BUILD_DIR := build
BIN_DIR := bin

# -------------------------------------------------------
# Source Files
# -------------------------------------------------------

SHARED_SRCS := $(shell find src/shared -name "*.c")
LOGGING_SRCS := $(shell find src/logging -name "*.c")

GAME_SRCS := $(shell find src/game -name "*.c")
EDITOR_SRCS := $(shell find src/editor -name "*.c")
DEVSTUDIO_SRCS := $(shell find src/devstudio -name "*.c")

# -------------------------------------------------------
# Objects
# -------------------------------------------------------

SHARED_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SHARED_SRCS))
LOGGING_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(LOGGING_SRCS))

GAME_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(GAME_SRCS))
EDITOR_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(EDITOR_SRCS))
DEVSTUDIO_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(DEVSTUDIO_SRCS))

ALL_OBJS := \
	$(SHARED_OBJS) \
	$(LOGGING_OBJS) \
	$(GAME_OBJS) \
	$(EDITOR_OBJS) \
	$(DEVSTUDIO_OBJS)

DEPS := $(ALL_OBJS:.o=.d)

# -------------------------------------------------------
# Default Target
# -------------------------------------------------------

.PHONY: all
all: game editor devstudio

# -------------------------------------------------------
# Executables
# -------------------------------------------------------

game: $(GAME_OBJS) $(LOGGING_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)== Building Game ========================$(RESET)"
	@mkdir -p $(BIN_DIR)
	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/game"
	@$(CC) $^ $(SDL_LIBS) -o $(BIN_DIR)/game
	@$(ECHO) "$(GREEN)✓ Game build complete.$(RESET)"

editor: $(EDITOR_OBJS) $(LOGGING_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)== Building Editor ======================$(RESET)"
	@mkdir -p $(BIN_DIR)
	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/editor"
	@$(CC) $^ $(SDL_LIBS) -o $(BIN_DIR)/editor
	@$(ECHO) "$(GREEN)✓ Editor build complete.$(RESET)"

devstudio: $(DEVSTUDIO_OBJS) $(LOGGING_OBJS) $(SHARED_OBJS)
	@$(ECHO) ""
	@$(ECHO) "$(CYAN)== Building DevStudio ===================$(RESET)"
	@mkdir -p $(BIN_DIR)
	@$(ECHO) "$(GREEN)[LD]$(RESET) $(BIN_DIR)/devstudio"
	@$(CC) $^ -o $(BIN_DIR)/devstudio
	@$(ECHO) "$(GREEN)✓ DevStudio build complete.$(RESET)"

# -------------------------------------------------------
# Compilation
# -------------------------------------------------------

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(ECHO) "$(BLUE)[CC]$(RESET) $<"
	@$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

-include $(DEPS)

# -------------------------------------------------------
# Utility Targets
# -------------------------------------------------------

.PHONY: clean rebuild

clean:
	@$(ECHO) "$(RED)[CLEAN]$(RESET) Removing build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@$(ECHO) "$(GREEN)✓ Clean complete.$(RESET)"

rebuild: clean all