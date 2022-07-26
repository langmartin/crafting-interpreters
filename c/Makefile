CC=gcc
CFLAGS=-O0 -std=c99 -pedantic -Wall -Werror
CFLAGS=-O0 -std=c99 -Wall

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

test: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

clean:
	rm -rf test $(OBJS)
