CC=gcc
CGLAGS = -g -Wall
TARGET	= sneaky

obj-m	+= $(TARGET).o
CURRENT = $(shell uname -r)
KDIR	= /lib/modules/$(CURRENT)/build

all: server client kernel

kernel:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

server: sneaky_server.c
	$(CC) $(CFLAGS) -o sneaky_server sneaky_server.c

client: sneaky_client.c
	$(CC) $(CFLAGS) -o sneaky_client sneaky_client.c

clean:
	rm sneaky_server sneaky_client
	$(MAKE) -C $(KDIR) M=$(PWD) clean
