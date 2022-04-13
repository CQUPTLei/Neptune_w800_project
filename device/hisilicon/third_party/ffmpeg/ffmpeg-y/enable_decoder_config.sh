echo `pwd`
CUR_DIR=$(pwd)
CFG_CHIP_TYPE=$1
CFG_OS_TYPE=$2

CFG_SDK_TOOLCHAIN=

configure_attr=" --prefix=./install \
    --enable-small \
    --disable-armv5te \
    --disable-armv6 \
    --disable-armv6t2 \
    --disable-linux-perf\
    --enable-shared \
    --disable-encoders \
    --disable-muxers \
    --disable-avfilter \
    --disable-swscale \
    --disable-demuxers \
    --enable-demuxer=mov \
    --enable-demuxer=mpegts \
    --disable-protocols \
    --enable-protocol=file \
    --disable-bsfs \
    --enable-bsf=h264_mp4toannexb \
    --enable-bsf=hevc_mp4toannexb \
    --disable-parsers \
    --enable-parser=hevc \
    --enable-parser=h264 \
    --disable-iconv \
    --disable-avdevice \
    --disable-network \
    --disable-decoders \
    --enable-decoder=h264 \
    --enable-decoder=hevc \
    --enable-decoder=mjpeg \
    --enable-asm \
    --disable-filters \
    --enable-inline-asm \
    --enable-neon \
    --disable-static \
    --disable-debug \
    --disable-doc \
    --disable-swresample \
    --disable-programs \
    --disable-symver \
    --target-os=linux \
    --enable-cross-compile \
    --disable-zlib \
    --disable-linux-perf \
    --cross-prefix=${CFG_SDK_TOOLCHAIN} "

export http_proxy=""
export https_proxy=""
export no_proxy=""

if [ "${CFG_CHIP_TYPE}" == "hi3518ev300" ]; then
echo "hi3518ev300 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --arch=arm --cpu=cortex-a7 "
echo ${configure_attr} --extra-cflags="-mfloat-abi=softfp -mfpu=neon-vfpv4"
./configure  ${configure_attr} --extra-cflags="-mfloat-abi=softfp -mfpu=neon-vfpv4 -fPIC -fstack-protector-all -s -ftrapv" --extra-ldflags="-fPIC -Wl,-z,relro,-z,now --shared"
fi

if [ "${CFG_CHIP_TYPE}" == "hi3516cv300" ]; then
echo "hi3516cv300 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --arch=arm --cpu=arm926ej-s --cross-prefix=${CFG_SDK_TOOLCHAIN} "
./configure  ${configure_attr} --extra-cflags="-nostdlib -nostdinc -fPIC -mlong-calls -fstack-protector-all -s -ftrapv" --extra-ldflags="-fPIC -Wl,-z,relro,-z,now --shared"
fi

