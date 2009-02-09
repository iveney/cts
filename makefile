CC=/usr/bin/gcc 
CFLAGS=-O0 -g -w
SRC=io.c dme.c main.c BufPlace.c connect.c
OBJ=$(SRC:%.c=%.o)

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(OBJ)
	$(CC) -o test $(CFLAGS) test.o io.o BufPlace.o connect.o -lm
	ctags -R *
clean:
	rm -rf $(OBJ) cts
