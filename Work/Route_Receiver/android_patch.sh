# !/bin/sh
# run it to get the Android version
# please place flute and keepalive binaries(in the android_bin folder) under /data/local/tmp in your Android device
# WARNING THE CHANGES CANNOT BE AUTOMATICALLY UNDONE!!! PLEASE KEEP YOU COPY FOR OTHER PLATEFORMS!!!
# PLEASE RUN THE SCRIPT ONLY ONCE FOR NON-ANDROID VERSION!!!

# sed -i 's/.\/flute/\/data\/local\/tmp\/flute/g' Receiver/Process.php
# sed -i 's/..\/..\/Config\/gensdp.sh/sh ..\/..\/Config\/gensdp.sh/g' Receiver/ReceiverConfig/setIP.php
# #insert before echo "done"
# sed -i '/echo "done";/i exec("sudo killall keepalive");\n$command= "sudo /data/local/tmp/keepalive ".$ip." &";\nexec($command);' Receiver/ReceiverConfig/setIP.php 

# Uncomment the following lines if you want to compile the binaries yourself, please change the paths according to your computer settings
# WARNING before this step, please make sure that you installed android NDK and toolchain correctly, 
# cross-compiled and installed curl, openssl, expat, zlib under the ANDROIDROOT!
# Afterwards, you can find the compiled flute binary under bin folder and keepalive under android_bin folder 

ANDROIDBIN="/opt/android-toolchain/arm-linux-androideabi/bin"
ANDROIDROOT="/opt/android-toolchain/sysroot"
sed -i "s|gcc|$ANDROIDBIN/gcc --sysroot=$ANDROIDROOT|g" alclib/Makefile
sed -i "s|gcc|$ANDROIDBIN/gcc --sysroot=$ANDROIDROOT|g" sdplib/Makefile
sed -i "s|gcc|$ANDROIDBIN/gcc --sysroot=$ANDROIDROOT|g" flutelib/Makefile
sed -i "s|-I/usr/local/ssl/include|-I$ANDROIDROOT/usr/include|g" flutelib/Makefile
sed -i "s|gcc|$ANDROIDBIN/gcc --sysroot=$ANDROIDROOT|g" flute/Makefile
sed -i "s|-I/usr/local/ssl/include \\\|-I$ANDROIDROOT/usr/include|g" flute/Makefile
sed -i "s|-I/usr/include||g" flute/Makefile
sed -i "s|-lpthread ||g" flute/Makefile
sed -i "s|-L/usr/local/ssl/lib|-L$ANDROIDROOT/usr/lib|g" flute/Makefile
# sed -i "s|gcc|$ANDROIDBIN/gcc --sysroot=$ANDROIDROOT|g" multis_flute/Makefile
# sed -i "s|-I/usr/local/ssl/include \\\|-I$ANDROIDROOT/usr/include|g" multis_flute/Makefile
# sed -i "s|-I/usr/include||g" multis_flute/Makefile
# sed -i "s|-lpthread ||g" multis_flute/Makefile
# sed -i "s|-L/usr/local/ssl/lib|-L$ANDROIDROOT/usr/lib|g" multis_flute/Makefile

# Source file changes
sed -i 's/#define SSM/\/\/#define SSM/g' alclib/defines.h
sed -i '/#define bcmp(a,b,n) memcmp(a,b,n)/a #else\n#define NEED_BCOPY\n#define bcmp(a,b,n) memcmp(a,b,n)' alclib/fec.c
sed -i 's/include <sys\/unistd.h>/include <unistd.h>/g' sdplib/osip_port.c
sed -i 's/fp = fopen64(filename, "rb");/fp = fopen(filename, "rb");/g' flutelib/mad_md5.c
sed -i 's/fp = fopen64(tx_file, "rb")/fp = fopen(tx_file, "rb")/g' flutelib/sender.c

# Build libwebsocket
cd libwebsockets-master/
sed -i 's/getdtablesize()/30000/g' lib/context.c
cd build/
rm -r *

cmake .. -DCMAKE_TOOLCHAIN_FILE=../cross-android.cmake \
-DZLIB_LIBRARY:FILEPATH=$ANDROIDROOT/usr/lib/libz.so -DZLIB_INCLUDE_DIR:PATH=$ANDROIDROOT/usr/include \
-DOPENSSL_CRYPTO_LIBRARY:FILEPATH=$ANDROIDROOT/usr/lib/libcrypto.so -DOPENSSL_EXECUTABLE:FILEPATH=$ANDROIDROOT/usr/bin/openssl \
-DOPENSSL_INCLUDE_DIR:PATH=$ANDROIDROOT/usr/include \
-DOPENSSL_SSL_LIBRARY:FILEPATH=$ANDROIDROOT/usr/lib/libssl.so \
-DCMAKE_C_FLAGS:STRING=--sysroot=$ANDROIDROOT

make

# sed -i "s|/usr/bin/ar|$ANDROIDBIN/ar|g" CMakeCache.txt
# sed -i "s|/usr/bin/cc|$ANDROIDBIN/gcc|g" CMakeCache.txt
# sed -i "s|CMAKE_C_FLAGS:STRING=|CMAKE_C_FLAGS:STRING=--sysroot=$ANDROIDROOT|g" CMakeCache.txt
# sed -i "s|/usr/bin/ld|$ANDROIDBIN/ld|g" CMakeCache.txt
# sed -i "s|/usr/bin/nm|$ANDROIDBIN/nm|g" CMakeCache.txt
# sed -i "s|/usr/bin/objcopy|$ANDROIDBIN/objcopy|g" CMakeCache.txt
# sed -i "s|/usr/bin/objdump|$ANDROIDBIN/objdump|g" CMakeCache.txt
# sed -i "s|/usr/bin/ranlib|$ANDROIDBIN/ranlib|g" CMakeCache.txt
# sed -i "s|/usr/bin/strip|$ANDROIDBIN/strip|g" CMakeCache.txt
# sed -i "s|/usr/lib/x86_64-linux-gnu|$ANDROIDROOT/usr/lib|g" CMakeCache.txt
# sed -i "s|/usr/bin/openssl|$ANDROIDROOT/usr/bin/openssl|g" CMakeCache.txt
# sed -i "s|/usr/include|$ANDROIDROOT/usr/include|g" CMakeCache.txt
# sed -i "s|/usr/bin/rpmbuild|$ANDROIDBIN/rpmbuild|g" CMakeCache.txt
# sed -i "s|_OPENSSL_PREFIX:INTERNAL=/usr|_OPENSSL_PREFIX:INTERNAL=$ANDROIDROOT/usr|g" CMakeCache.txt
# Remove libwebsocket.so
rm lib/*.so
cd ../..

# Compile flute
make clean
make

# Compile keepalive
cd android_bin
$ANDROIDBIN/gcc keepalive.c -o keepalive