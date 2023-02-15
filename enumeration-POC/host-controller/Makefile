CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -Werror
LIBS=-lusb-1.0

BUILD_DIR=./build

SOURCES := $(wildcard ./*.c)
OBJS := $(SOURCES:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(addprefix $(BUILD_DIR)/,$^) -o $(BUILD_DIR)/host-controller $(LIBS)

%.o: %.c build
	$(CC) $(FLAGS) $< -c -o $(BUILD_DIR)/$@

build:
	mkdir -p ./build

clean:
	rm -rf ./build
