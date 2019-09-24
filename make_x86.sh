source `find_init_env.sh`
export SYSROOT=$X86_SYSROOT
export APPEND_CFLAGS=-m32\ --sysroot=$SYSROOT
export APPEND_LDFLAGS=-m32\ --sysroot=$SYSROOT

make -j8
