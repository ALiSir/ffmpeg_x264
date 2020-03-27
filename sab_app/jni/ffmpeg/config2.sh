#!/bin/sh
export TMPDIR="/tmp/"
export NDKROOT="/home/ldm/Desktop/test/android-ndk-r9"
PREBUILT=$NDKROOT/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86
./configure --target-os=linux
--arch=arm  
--cpu=armv7-a
--enable-cross-compile
--cc=$PREBUILT/bin/arm-linux-androideabi-gcc
--cross-prefix=$PREBUILT/bin/arm-linux-androideabi-
--nm=$PREBUILT/bin/arm-linux-androideabi-nm
--extra-cflags="-fPIC -DANDROID -mfpu=neon -mfloat-abi=softfp -I$NDKROOT/platforms/android-9/arch-arm/usr/include"
--enable-asm
--disable-yasm
--enable-static
--disable-shared
--enable-small
--enable-gpl
--enable-version3
--enable-nonfree
--enable-neon
--disable-ffmpeg
--disable-ffplay
--disable-ffserver
--disable-ffprobe
--prefix=/home/ldm/Desktop/test/ffmpeg-android-bin
--extra-ldflags="-Wl,-T,$PREBUILT/arm-linux-androideabi/lib/ldscripts/armelf_linux_eabi.x -Wl,-rpath-link=$NDKROOT/platforms/android-9/arch-arm/usr/lib -L$NDKROOT/platforms/android-9/arch-arm/usr/lib -nostdlib $PREBUILT/lib/gcc/arm-linux-androideabi/4.6/crtbegin.o $PREBUILT/lib/gcc/arm-linux-androideabi/4.6/crtend.o -lc -lm -ldl"
