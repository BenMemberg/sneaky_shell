CC=gcc
CGLAGS = -g -Wall
TARGET	= kernel

obj-m	+= $(TARGET).o
CURRENT = $(shell uname -r)
KDIR	= /lib/modules/$(CURRENT)/build

all: server client kernel

kernel:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm server client
	$(MAKE) -C $(KDIR) M=$(PWD) clean
