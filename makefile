CC = gcc
CFLAGS = -O3
OBJ = io.o dme.o main.o

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm
main.o : main.c ds.h
	$(CC) -c $(CFLAGS) main.c
io.o : io.c ds.h
	$(CC) -c $(CFLAGS) io.c
dme.o : dme.c ds.h
	$(CC) -c $(CFLAGS) dme.c
clean:
	rm $(OBJ) cts
