CC=c99
CFLAGS=-I.
DEPS = header.h
OBJ = main.o operations.o intExt.o printIntExt.o parseExpression.o

all: calculate

clear:
	rm *.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

calculate: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)