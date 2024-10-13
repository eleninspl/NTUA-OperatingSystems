CC = gcc
CFLAGS = -Wall

all: parent child

parent: parent.o
	$(CC) $(CFLAGS) -o parent parent.o

parent.o: parent.c
	$(CC) $(CFLAGS) -c parent.c

child: child.o
	$(CC) $(CFLAGS) -o child child.o

child.o: child.c
	$(CC) $(CFLAGS) -c child.c

clean:
	rm -f parent child *.o
