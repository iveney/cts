CC=/usr/bin/gcc 
CFLAGS=-g -Wall -pg
SRC=io.c dme.c bufplace.c connect.c util.c main.c
OBJ=$(SRC:%.c=%.o)
TSTOBJ=test.o io.o bufplace.o connect.o util.o
CTS=cts
TEST=test
DEBUG=debug
EXE=$(CTS) $(TEST) $(DEBUG)

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(OBJ) 
	ctags -R *
	$(CC) -c $(CFLAGS) connect.c 
	$(CC) -c $(CFLAGS) test.c
	$(CC) -o $(TEST) $(CFLAGS) $(TSTOBJ) -lm
	$(CC) -c $(CFLAGS) connect.c -DDEBUG
	$(CC) -o $(DEBUG) $(CFLAGS) -lm -DDEBUG $(TSTOBJ)

clean:
	rm -rf *.o $(EXE)
