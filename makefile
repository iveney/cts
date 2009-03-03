CC=/usr/bin/gcc 
CFLAGS= -w 
SRC=io.c segment.c dme.c main.c bufplace.c connect.c preprocess.c util.c
OBJ=$(SRC:%.c=%.o)
TSTOBJ=test.o io.o bufplace.o connect.o util.o preprocess.o segment.o dme.o
CTS=cts
TEST=test
DEBUG=debug
EXE=$(CTS) $(TEST) $(DEBUG)

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(OBJ) 
	ctags -R *.c *.h
	$(CC) -c $(CFLAGS) test.c
	$(CC) -c $(CFLAGS) connect.c 
	$(CC) -o $(TEST) $(CFLAGS) $(TSTOBJ) -lm
	$(CC) -c $(CFLAGS) connect.c -DDEBUG
	$(CC) -o $(DEBUG) $(CFLAGS) $(TSTOBJ) -lm -DDEBUG 

clean:rm
	rm -rf *.o $(EXE)

rm:
	rm -rf rect_* tree_*
