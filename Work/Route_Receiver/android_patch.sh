#!/bin/sh
#run it to get the Android version
#please place flute and keepalive binaries(in the android_bin folder) under /data/local/tmp in your Android device
#THE CHANGES CANNOT BE AUTOMATICALLY UNDONE!!! PLEASE KEEP YOU COPY FOR OTHER PLATEFORMS!!!

sed -i 's/.\/flute/\/data\/local\/tmp\/flute/g' Receiver/Process.php
sed -i 's/..\/..\/Config\/gensdp.sh/sh ..\/..\/Config\/gensdp.sh/g' Receiver/ReceiverConfig/setIP.php
#insert before echo "done"
sed -i '/echo "done";/i exec("sudo killall keepalive");\n$command= "sudo /data/local/tmp/keepalive ".$ip." &";\nexec($command);' Receiver/ReceiverConfig/setIP.php 

# Uncomment the following lines if you want to compile the binaries yourself, please change the paths according to your computer settings
# ATTENTION: before this step, please make sure that you installed android NDK and toolchain correctly, 
# cross-compiled and installed curl, openssl, expat, zconf, zlib under the Androidroot, 
# cross-compiled libwebsocket correctly! (CMakeCache.txt under build folder needs to be modified)
# Afterwards, you can find the compiled flute binary under bin folder and keepalive under android_bin folder 

# Androidgcc="/opt/android-toolchain/arm-linux-androideabi/bin/gcc"
# Androidroot="/opt/android-toolchain/sysroot"
# sed -i "s|gcc|$Androidgcc --sysroot=$Androidroot|g" alclib/Makefile
# sed -i "s|gcc|$Androidgcc --sysroot=$Androidroot|g" sdplib/Makefile
# sed -i "s|gcc|$Androidgcc --sysroot=$Androidroot|g" flutelib/Makefile
# sed -i "s|-I/usr/local/ssl/include|-I$Androidroot/usr/include|g" flutelib/Makefile
# sed -i "s|gcc|$Androidgcc --sysroot=$Androidroot|g" flute/Makefile
# sed -i "s|-I/usr/local/ssl/include \\\|-I$Androidroot/usr/include|g" flute/Makefile
# sed -i "s|-I/usr/include||g" flute/Makefile
# sed -i "s|-lpthread ||g" flute/Makefile
# sed -i "s|-L/usr/local/ssl/lib|-L$Androidroot/usr/lib|g" flute/Makefile
# sed -i "s|gcc|$Androidgcc --sysroot=$Androidroot|g" multis_flute/Makefile
# sed -i "s|-I/usr/local/ssl/include \\\|-I$Androidroot/usr/include|g" multis_flute/Makefile
# sed -i "s|-I/usr/include||g" multis_flute/Makefile
# sed -i "s|-lpthread ||g" multis_flute/Makefile
# sed -i "s|-L/usr/local/ssl/lib|-L$Androidroot/usr/lib|g" multis_flute/Makefile
# 
# # Source file changes
# sed -i 's/#define SSM/\/\/#define SSM/g' alclib/defines.h
# sed -i '/#define bcmp(a,b,n) memcmp(a,b,n)/a #else\n#define NEED_BCOPY\n#define bcmp(a,b,n) memcmp(a,b,n)' alclib/fec.c
# sed -i 's/include <sys\/unistd.h>/include <unistd.h>/g' sdplib/osip_port.c
# sed -i 's/fp = fopen64(filename, "rb");/fp = fopen(filename, "rb");/g' flutelib/mad_md5.c
# sed -i 's/fp = fopen64(filename, "rb");/fp = fopen(filename, "rb");/g' flutelib/sender.c
#
# # Compile flute
# make clean
# make
# 
# # Compile keepalive
# cd android_bin
# $Androidgcc keepalive.c -o keepalive