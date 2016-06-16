CC = gcc

CFLAGS += -Wall -O3 -g -fPIC -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 -D_GNU_SOURCE \
	-I/usr/local/ssl/include -I../libwebsockets-master/build -I../libwebsockets-master/lib

SOURCES = flute.c sender.c receiver.c fdt.c efdt.c fdt_gen.c mad_md5.c uri.c getdnsname.c mad_zlib.c \
	padding_encoding.c parse_args.c mad_utf8.c http_file_repair.c flute_file_repair.c apd.c \
	display.c test-server.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: ../lib/libflutelib.a

../lib/libflutelib.a: $(OBJECTS)
	@echo "creating library $@..."
	ar rc $@ $(OBJECTS) ../lib/libsdp.a ../lib/libalc.a
	ranlib $@

clean:
	rm -f ../lib/libflutelib.a *.o *~

