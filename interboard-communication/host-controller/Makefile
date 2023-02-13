
all: build main.o
	cc ./build/main.o -o ./build/host-controller -lusb-1.0

build:
	mkdir -p ./build

main.o: build
	cc -c main.c -o ./build/main.o

clean:
	rm -rf ./build
