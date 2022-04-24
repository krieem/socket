CC=gcc
CFLAGS=-g -pthread

all: client server

client: client.c
		$(CC) $(CFLAGS) -o $@ $^

server: server.c
		$(CC) $(CFLAGS) -o $@ $^

clean:
		rm -rf *.dSYM client server