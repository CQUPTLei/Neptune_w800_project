echo `pwd`
CUR_DIR=$(pwd)
CFG_CHIP_TYPE=$1
CFG_OS_TYPE=$2
CFG_LINUX_COMPILER_VER=$3
CFG_OHOS_BUILD_PATH=$4

echo "CFG_CHIP_TYPE=${CFG_CHIP_TYPE} CFG_OS_TYPE=${CFG_OS_TYPE} CFG_LINUX_COMPILER_VER=${CFG_LINUX_COMPILER_VER} CFG_OHOS_BUILD_PATH=${CFG_OHOS_BUILD_PATH}"
if [ "${CFG_OS_TYPE}" == "ohos" ]; then
CFG_SDK_TOOLCHAIN=${CFG_OHOS_BUILD_PATH}/
else
CFG_SDK_TOOLCHAIN=arm-${CFG_LINUX_COMPILER_VER}-linux-
fi

configure_attr=" --prefix=./install \
    --disable-autodetect \
    --enable-cross-compile \
    --disable-doc \
    --disable-htmlpages \
    --target-os=linux \
    --enable-shared \
    --disable-static \
    --disable-debug \
    --disable-iconv  \
    --enable-small \
    --disable-network \
    --disable-filters \
    --disable-devices \
    --disable-programs \
    --disable-swresample \
    --disable-swscale \
    --disable-avdevice \
    --disable-postproc \
    --disable-avfilter \
    --disable-protocols \
    --disable-pthreads \
    --disable-runtime-cpudetect \
    --disable-everything   \
    --enable-pic   \
    --enable-protocol=file \
    --disable-muxers \
    --enable-demuxer=mov\
    --enable-demuxer=mpegts\
    --enable-parser=hevc \
    --enable-parser=h264 \
    --disable-neon \
    --disable-inline-asm \
    --disable-asm \
    --disable-armv6 \
    --disable-armv6t2 \
    --disable-armv5te \
    --disable-vfp \
    --disable-hardcoded-tables \
    --disable-mediacodec \
    --enable-bsf=h264_mp4toannexb \
    --enable-bsf=hevc_mp4toannexb \
    --disable-pixelutils \
    --enable-demuxer=wav \
    --disable-gpl \
    --disable-zlib \
    --disable-w32threads --disable-os2threads --disable-alsa --disable-appkit --disable-avfoundation \
    --disable-bzlib --disable-coreimage --disable-iconv --disable-libxcb --disable-libxcb-shm \
    --disable-libxcb-xfixes --disable-libxcb-shape --disable-lzma --disable-sndio --disable-schannel \
    --disable-sdl2 --disable-securetransport --disable-xlib --disable-amf --disable-audiotoolbox \
    --disable-cuda-llvm --disable-cuvid --disable-nvdec --disable-nvenc --disable-vaapi --disable-vdpau \
    --disable-videotoolbox --disable-ossfuzz --disable-swscale-alpha \
    --disable-valgrind-backtrace \
    --disable-linux-perf "


if [ "${CFG_CHIP_TYPE}" == "hi3559"  -o  "${CFG_CHIP_TYPE}" == "hi3556" ]; then
echo "hi3559/hi3556 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --cpu=cortex-a7 --arch=armv7-a --cross-prefix=${CFG_SDK_TOOLCHAIN} "
fi

if [ "${CFG_CHIP_TYPE}" == "hi3559v200" ]; then
echo "hi3559v200 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --cpu=cortex-a7 --arch=armv7-a --cross-prefix=${CFG_SDK_TOOLCHAIN} "
fi

if [ "${CFG_CHIP_TYPE}" == "hi3516dv300" ]; then
echo "hi3516dv300 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --cpu=cortex-a7 --arch=armv7-a --cross-prefix=${CFG_SDK_TOOLCHAIN} "
fi

if [ "${CFG_CHIP_TYPE}" == "hi3518ev300" ]; then
echo "hi3518ev300 =? ${CFG_CHIP_TYPE}"
configure_attr+=" --cpu=cortex-a7 --arch=armv7-a --cross-prefix=${CFG_SDK_TOOLCHAIN} "
fi

echo ${configure_attr}
export http_proxy=""
export https_proxy=""
export no_proxy=""

./configure  ${configure_attr} --extra-cflags="-mfloat-abi=softfp -mfpu=neon-vfpv4 -fPIC -fstack-protector-all -s -ftrapv" --extra-ldflags="-Wl,-z,relro,-z,now -fPIC"
