LDLIBS=-lncurses

all: example

example: example.o console.o

clean:
	rm -rf example *.o

