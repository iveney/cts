CC=/usr/bin/gcc 
CFLAGS=-g -Wall
SRC=io.c dme.c bufplace.c connect.c util.c main.c
OBJ=$(SRC:%.c=%.o)
TSTOBJ=test.o io.o bufplace.o connect.o util.o

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(OBJ) $(debug)
	$(CC) -c $(CFLAGS) connect.c 
	$(CC) -c $(CFLAGS) test.c
	$(CC) -o test $(CFLAGS) $(TSTOBJ) -lm
	$(CC) -c $(CFLAGS) connect.c -DDEBUG
	$(CC) -o debug $(CFLAGS) -lm -DDEBUG $(TSTOBJ)
	ctags -R *

clean:
	rm -rf $(OBJ) cts
