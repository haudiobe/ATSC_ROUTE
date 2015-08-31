CC = gcc

CFLAGS += -Wall -g -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 \
	-I/usr/local/ssl/include

LFLAGS += -L../lib -lflutelib -lalc -lsdp \
	-lexpat -lpthread -lm -lz \
	-L/usr/local/ssl/lib -lcrypto -lcurl

SOURCES = main.c repair_sender.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: repair_sender

repair_sender: $(OBJECTS)
	$(CC) -o ../bin/repair_sender $(OBJECTS) $(LFLAGS)

clean:
	rm -f ../bin/repair_sender *.o *~

main.c: repair_sender.h ../flutelib/flute.h
