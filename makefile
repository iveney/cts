CC=/usr/bin/gcc 
CFLAGS=-g -Wall
SRC=io.c dme.c main.c bufplace.c connect.c
OBJ=$(SRC:%.c=%.o)

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(OBJ) $(debug)
	$(CC) -c $(CFLAGS) connect.c 
	$(CC) -c $(CFLAGS) test.c
	$(CC) -o test $(CFLAGS) test.o io.o bufplace.o connect.o -lm
	$(CC) -c $(CFLAGS) connect.c -DDEBUG
	$(CC) -o debug $(CFLAGS) test.o io.o bufplace.o connect.o -lm -DDEBUG
	ctags -R *
clean:
	rm -rf $(OBJ) cts
