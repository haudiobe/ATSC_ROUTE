CC = gcc

CFLAGS += -Wall -O3 -g -fPIC -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE \
	-D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 -D_GNU_SOURCE

SOURCES = alc_channel.c alc_hdr.c alc_rx.c alc_session.c alc_socket.c alc_tx.c \
	lct_hdr.c mad.c transport.c blocking_alg.c null_fec.c xor_fec.c fec.c rs_fec.c \
	mad_rlc.c alc_list.c

OBJECTS = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: ../lib/libalc.a

../lib/libalc.a: $(OBJECTS)
	@echo "creating library $@..."
	ar rc $@ $(OBJECTS)
	ranlib $@

clean:
	rm -f ../lib/libalc.a *.o *~

