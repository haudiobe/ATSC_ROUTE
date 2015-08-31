CC = gcc

CFLAGS += -Wall -O3 -g -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 \
	-I../flutelib \
	-I/usr/local/ssl/include

LFLAGS += -L../lib -lflutelib -lalc -lsdp \
	-lexpat -lpthread -lm -lz -lcurl \
	-L/usr/local/ssl/lib/ -lcrypto

SOURCES = main.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: multis_flute

multis_flute: $(OBJECTS)
	$(CC) -o ../bin/multis_flute $(OBJECTS) $(LFLAGS)

clean:
	rm -f ../bin/multis_flute *.o *~

main.c: ../flutelib/flute.h ../flutelib/http_file_repair.h \
	../flutelib/flute_file_repair.h ../flutelib/apd.h
