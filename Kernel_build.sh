#!/bin/bash

export PATH=$PWD/prebuilts/build-tools/path/linux-x86:$PATH
export ARCH=arm64
export CLANG_TRIPLE=aarch64-linux-gun-
export CROSS_COMPIILE_COMPAT=arm-linux-androideabi-
export SUBARCH=arm64
export CROSS_COMPILE=aarch64-linux-androidkernel-
export LD_LIBRARY_PATH=prebuilts/clang/host/linux-x86/clang-r383902/lib64:$$LD_LIBRARY_PATH

cd kernel-4.14/
rm -rf out/

PATH_CC=$PWD/../prebuilts/clang/host/linux-x86/clang-r383902/bin/clang
PATH_CROSS_COMPILE=$PWD/../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9.1/bin/aarch64-linux-androidkernel-

make ARCH=arm64 CC=$PATH_CC O=out tb8185p3_64_defconfig
make ARCH=arm64 CROSS_COMPILE=$PATH_CROSS_COMPILE CLANG_TRIPLE=aarch64-linux-gnu- CC=$PATH_CC O=out -j8
