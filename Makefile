CC=/usr/bin/gcc 
CFLAGS=-Wall -g 
SRC=io.c segment.c dme.c main.c bufplace.c connect.c preprocess.c util.c fast_path.c crt_path.c heap.c
OBJ=$(SRC:%.c=%.o)
TSTOBJ=io.o bufplace.o connect.o util.o preprocess.o segment.o dme.o 
CTS=cts
TEST=test
DEBUG=debug
CLUSTER=cluster
EXE=$(CTS) $(TEST) $(DEBUG) $(CLUSTER)

cts : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

test: $(TSTOBJ) 
	ctags -R *.c *.h
	$(CC) -c $(CFLAGS) test.c
	$(CC) -c $(CFLAGS) connect.c 
	$(CC) -o $(TEST) $(CFLAGS) $(TSTOBJ) test.o -lm
	$(CC) -c $(CFLAGS) connect.c -DDEBUG
	$(CC) -o $(DEBUG) $(CFLAGS) $(TSTOBJ) test.o -lm -DDEBUG 

cluster:$(TSTOBJ) cluster.c tst_clt.c
	$(CC) -c $(CFLAGS) cluster.c
	$(CC) -c $(CFLAGS) tst_clt.c
	$(CC) -o $(CLUSTER) $(CFLAGS) $(TSTOBJ) tst_clt.o cluster.o -lm

clean:rm
	rm -rf *.o $(EXE)

rm:
	rm -rf rect_* tree_*
