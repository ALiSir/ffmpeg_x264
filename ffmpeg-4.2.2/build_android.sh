#!/bin/bash
NDK=/home/a/decode/android-ndk-r14b-linux-x86_64/android-ndk-r14b
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64

function build_one
{
	./configure \
		--prefix=./android \
		--enable-cross-compile \
		--target-os=android \
		--arch=arm \
		--sysroot=$NDK/platforms/android-19/arch-arm/ \
		--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
		--cc=$TOOLCHAIN/bin/arm-linux-androideabi-gcc \
		--enable-gpl \
		--extra-cflags=-I/home/a/x264/build/include \
		--extra-ldflags=-L/home/a/x264/build/lib \
		--extra-libs=-ldl \
		--enable-libx264 \
		--enable-decoder=h264_mediacodec \
		--disable-shared \
		--enable-static \
${EXTRA_BUILD_CONFIGURATION_FLAGS} \
--disable-runtime-cpudetect \
--disable-programs \
--disable-muxers \
--disable-encoders \
--disable-postproc \
--disable-swresample \
--disable-avfilter \
--disable-doc \
--disable-debug \
--disable-pthreads \
--disable-network \
--disable-bsfs \
${DECODERS_TO_ENABLE}
}
build_one
