CC=gcc
CFLAGS=-Wall -Werror -std=c99 -pedantic -g
OBJ=main.o perms.o des.o
EXEC=des

des: $(OBJ) 
	$(CC) $(OBJ) -o des $(LFLAGS)

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o main.o

perms.o: perms.h perms.c
	$(CC) $(CFLAGS) -c perms.c -o perms.o

des.o: des.h des.c
	$(CC) $(CFLAGS) -c des.c -o des.o

clean:
	rm -f $(EXEC) *.o *~

format:
	find . -name "*.c" | xargs indent -par -br -brf -brs -kr -ci2 -cli2 -i2 -l80 -nut
	find . -name "*.h" | xargs indent -par -br -brf -brs -kr -ci2 -cli2 -i2 -l80 -nut
