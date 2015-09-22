CC = /opt/android-toolchain/arm-linux-androideabi/bin/gcc --sysroot=/opt/android-toolchain/sysroot
#CC = ccmalloc g++

CFLAGS += -Wall -O3 -g -DLINUX -D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 -D_GNU_SOURCE \
	-I../flutelib \
	-I/opt/android-toolchain/sysroot/usr/include
	

LFLAGS += -L../lib -lflutelib -lalc -lsdp \
	-lexpat -lm -lz -lcurl -L../libwebsockets-master/build/lib -lwebsockets \
	-L/opt/android-toolchain/sysroot/usr/lib -lcrypto -lssl

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
