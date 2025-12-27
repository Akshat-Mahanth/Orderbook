CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude

SRC := \
    src/hashmap.c \
    src/queue.c \
    src/heap.c \
    src/orderbook.c \
    src/depth.c \
    src/print.c \
    src/trade_log.c \
    src/candle_builder.c \
    src/agent_random.c \
    src/main.c


OBJ := $(SRC:src/%.c=build/%.o)

BIN := main.exe

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN) output/*.csv output/*.png
