CC = gcc -g -w
CFLAGS = -O3
OBJ = io.o dme.o main.o BufPlace.o

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm
main.o : main.c ds.h
	$(CC) -c $(CFLAGS) main.c
io.o : io.c ds.h
	$(CC) -c $(CFLAGS) io.c
dme.o : dme.c ds.h
	$(CC) -c $(CFLAGS) dme.c
bufplace.o : BufPlace.c ds.h
	$(CC) -c $(CFLAGS) BufPlace.c
clean:
	rm $(OBJ) cts
