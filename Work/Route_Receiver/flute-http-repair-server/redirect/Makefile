CC = gcc

CFLAGS += -Wall -g -DLINUX -D__VXWORKS_OS__ -DHAVE_CONFIG_H \
	-I/usr/local/ssl/include

LFLAGS += -L../../lib -lflutelib

SOURCES = redirect.c ../mbms-rel6-flute-repair/helpers.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: redirect.cgi

redirect.cgi: $(OBJECTS)
	$(CC) -o ../bin/redirect.cgi $(OBJECTS) $(LFLAGS)
clean:
	rm -f ../bin/redirect.cgi *.o *~ ../mbms-rel6-flute-repair/helpers.o

redirect.c: ../mbms-rel6-flute-repair/helpers.h ../../flutelib/getdnsname.h
