# ===============================
# Compiler & tools
# ===============================
CC      := gcc
RM      := rm -rf
MKDIR   := mkdir -p

# ===============================
# Directories
# ===============================
SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

# ===============================
# Flags
# ===============================
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2 -I$(INC_DIR)

# ===============================
# Sources
# ===============================
SRCS := \
    src/hashmap.c \
    src/queue.c \
    src/heap.c \
    src/orderbook.c \
    src/depth.c \
    src/print.c \
    src/main.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# ===============================
# Output
# ===============================
TARGET := main.exe

# ===============================
# Rules
# ===============================
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

clean:
	$(RM) $(BUILD_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
