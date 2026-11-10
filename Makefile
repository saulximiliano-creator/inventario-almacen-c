CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: inventario

inventario: src/main.c src/inventario.c src/inventario.h
	$(CC) $(CFLAGS) -o inventario src/main.c src/inventario.c

test: tests/test_inventario.c src/inventario.c src/inventario.h
	$(CC) $(CFLAGS) -o test_inventario tests/test_inventario.c src/inventario.c
	./test_inventario

clean:
	rm -f inventario test_inventario

.PHONY: all test clean
