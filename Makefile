CC=gcc
CFLAGS=-Wall -Werror -std=c99 -pedantic -g
OBJ=perms.o des.o
EXEC=test des

des: main.o $(OBJ)
	$(CC) main.o $(OBJ) -o des

test: test.o $(OBJ)
	$(CC) test.o $(OBJ) -o test

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

test.o: test.c
	$(CC) $(CFLAGS) -c test.c -o test.o

perms.o: perms.h perms.c
	$(CC) $(CFLAGS) -c perms.c -o perms.o

des.o: des.h des.c
	$(CC) $(CFLAGS) -c des.c -o des.o

clean:
	rm -f $(EXEC) *.o *~ *.dec

format:
	find . -name "*.c" | xargs indent -par -br -brf -brs -kr -ci2 -cli2 -i2 -l80 -nut
	find . -name "*.h" | xargs indent -par -br -brf -brs -kr -ci2 -cli2 -i2 -l80 -nut
