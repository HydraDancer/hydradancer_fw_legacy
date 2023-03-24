
ifeq ($(OS), Windows_NT)
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O2 `pkg-config --cflags libusb-1.0` $(INCLUDES)
LDFLAGS = -L/mingw64/lib -I/mingw64/include/libusb-1.0 -lusb-1.0
else
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O2 `pkg-config --cflags libusb-1.0` $(INCLUDES)
LDFLAGS = `pkg-config --libs libusb-1.0`
endif

BUILD_DIR=./build

SOURCES := $(wildcard ./*.c)
OBJS := $(SOURCES:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(addprefix $(BUILD_DIR)/,$^) -o $(BUILD_DIR)/host-controller $(LIBS) $(LDFLAGS)

%.o: %.c build
	$(CC) $(CFLAGS) $(LDFLAGS) $<  -c -o  $(BUILD_DIR)/$@

build:
	mkdir -p ./build
ifeq ($(OS), Windows_NT)
	cp C:/msys64/mingw64/bin/libusb-1.0.dll ./build/
endif
clean:
	rm -rf ./build
