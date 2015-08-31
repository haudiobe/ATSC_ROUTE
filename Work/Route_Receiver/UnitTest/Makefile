CC = g++

CFLAGS += -Wall -O3 -g -DLINUX -D__VXWORKS_OS__ -DHAVE_CONFIG_H \
	-D_LARGE_FILE_API -D_LARGEFILE64_SOURCE -D_ISOC99_SOURCE -D_XOPEN_SOURCE=500 \
	-I../flutelib -I/usr/local/ssl/include -I/usr/local/include/boost-1_33_1\
	-I.

LFLAGS += -L../lib -lflutelib -lalc -lsdp \
	-lexpat -lpthread -lm -lz \
	-L/usr/local/ssl/lib/ -lcrypto -L/usr/local/lib \
	-lboost_thread-gcc-mt-1_33_1 -lboost_filesystem-gcc-mt-1_33_1 \
	-lcppunit -lcurl

SOURCES = CppUnitTestMain.cpp CppUnitTestSuite.cpp $(wildcard Tests/FluteLib/*.cpp) $(wildcard TestSupport/*.cpp)

OBJECTS = $(SOURCES:%.cpp=%.o)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

all: unittest

unittest: $(OBJECTS)
	$(CC) -o ../bin/flute_unittest $(OBJECTS) $(LFLAGS)

clean:
	rm -f ../bin/flute_unittest *.o *~ TestSupport/*.o TestSupport/*~ Tests/FluteLib/*.o Tests/FluteLib/*~


