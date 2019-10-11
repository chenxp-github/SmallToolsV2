export CROSS_COMPILE=arm-linux-androideabi-
export NO_PTHREAD=1
export APPEND_CFLAGS=-DANDROID_NDK
make -j8
