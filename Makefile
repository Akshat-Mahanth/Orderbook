CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2
INCLUDES:= -Isrc -Iinclude
LDFLAGS := -lrt -pthread

SRC_DIR := src
OBJ_DIR := build

SRCS := \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/agent_random.c \
    $(SRC_DIR)/agent_thread.c \
    $(SRC_DIR)/dispatcher.c \
    $(SRC_DIR)/order_queue.c \
    $(SRC_DIR)/orderbook.c \
    $(SRC_DIR)/snapshot.c \
    $(SRC_DIR)/shm.c \
    $(SRC_DIR)/queue.c \
    $(SRC_DIR)/heap.c \
    $(SRC_DIR)/hashmap.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET := sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean

