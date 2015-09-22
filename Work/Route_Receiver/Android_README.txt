 How to use Route Receiver in Android:

    Copy the whole Route_Receiver folder to you server root in Android, then move the binaries (flute, keepalive) in android_bin folder to /data/local/tmp/.
    Change the settings for chrome browser: Open chrome://flags, then go to: Disable gesture requirement for media playback. Android Click on enable, then restart the browser.
    Find the receiver in your localhost and use it the same way as the desktop version

 How to compile Route Receiver in Android (not recommended):

    If you encounter a problem with the existing binaries, or out of whatever reason you want to recompile the binaries. Please follow the steps:
    1. make sure that you installed android NDK and toolchain correctly, and the libs like gcc are located in "/opt/android-toolchain/arm-linux-androideabi/bin", 
    while the user libs, include files etc. are under "/opt/android-toolchain/sysroot"(ANDROIDROOT)
    2. make sure that you cross-compiled and installed curl, openssl, expat, zlib under the ANDROIDROOT!
    3. compile flute and keepalive. You can save the following command as shell script and run directly in Route_Receiver
    
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
        rm lib/*.so
        cd ../..

        # Compile flute
        make clean
        make

        # Compile keepalive
        cd android_bin
        rm keepalive
        $ANDROIDBIN/gcc keepalive.c -o keepalive
    
    4. In the end, you can find the compiled flute binary in bin folder and keepalive in android_bin folder. Please copy them to /data/local/tmp and modify the browser settings
    the same as stated above. 