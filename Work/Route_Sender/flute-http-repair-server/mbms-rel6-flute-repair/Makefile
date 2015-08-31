CC = gcc

CFLAGS += -Wall -g -DLINUX -D__VXWORKS_OS__ -DHAVE_CONFIG_ -D_LARGE_FILE_API \
	-D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 \
	-I/usr/local/ssl/include

LFLAGS += -L../../lib -lalc -lflutelib \
	-lexpat -lm -lz

SOURCES = main.c decode_query_string.c parse_query_string.c add_fec_plid.c helpers.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: mbms-rel6-FLUTE-repair

mbms-rel6-FLUTE-repair: $(OBJECTS)
	$(CC) -o ../bin/mbms-rel6-FLUTE-repair $(OBJECTS) $(LFLAGS)
clean:
	rm -f ../bin/mbms-rel6-FLUTE-repair *.o *~

main.c: decode_query_string.h parse_query_string.h add_fec_plid.h helpers.h \
	../../flutelib/fdt.h ../../flutelib/uri.h ../../flutelib/mad_utf8.h
