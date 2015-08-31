CC = gcc

CFLAGS += -Wall -O3 -g -fPIC -DLINUX -DSTDC_HEADERS -DHAVE_CTYPE_H -DHAVE_STRING_H \
	-DHAVE_SYS_TYPES_H -DHAVE_SYS_TIME_H -DHAVE_TIME_H -DHAVE_FCNTL_H \
	-DHAVE_UNISTD_H -DHAVE_SYS_UNISTD_H -DHAVE_SYS_SELECT_H -DHAVE_PTH_PTHREAD_H

SOURCES = osip_port.c osip_list.c sdp_accessor.c sdp_message.c sdplib.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: ../lib/libsdp.a

../lib/libsdp.a: $(OBJECTS)
	@echo "creating library $@..."
	ar rc $@ $(OBJECTS)
	ranlib $@

clean:
	rm -f ../lib/libsdp.a *.o *~
