CC = gcc
#CC = ccmalloc g++

CFLAGS += -Wall -g3 -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 -D_GNU_SOURCE \
	-I../flutelib \
	-I/usr/local/ssl/include \
	-I/usr/include

LFLAGS += -L../lib -lflutelib -lalc -lsdp \
	-lexpat -lpthread -lm -lz -lcurl -L../libwebsockets-master/build/lib -lwebsockets \
	-L/usr/local/ssl/lib -lcrypto -lssl

SOURCES = main.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: flute

flute: $(OBJECTS)
	$(CC) -o ../bin/flute $(OBJECTS) $(LFLAGS)

clean:
	rm -f ../bin/flute *.o *~

main.c: ../flutelib/flute.h ../flutelib/http_file_repair.h \
	../flutelib/flute_file_repair.h ../flutelib/apd.h
