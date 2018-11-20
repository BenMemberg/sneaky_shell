#makefile for hello.c

CC=gcc
CGLAGS = -g -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server presServer.c

client: client.c
	$(CC) $(CFLAGS) -o client presClient.c

clean:
	rm server client
