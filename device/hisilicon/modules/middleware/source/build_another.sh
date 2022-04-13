#!/bin/bash
# Copyright (c) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Compile middleware project, this is the entrance script

# error out on errors
set -e
OUT_DIR="$1"
BOARD_NAME="$2"
HOS_KERNEL_TYPE="$3"
HOS_BUILD_COMPILER="$4"
OHOS_BUILD_PATH="$5"
function main(){
    CUR_DIR=$(cd $(dirname "$0");pwd)
    ROOT_DIR=$CUR_DIR/../../../../..
    unset OS_TYPE
    COMPILER="llvm"
    if [ "$HOS_KERNEL_TYPE" = "liteos_a" ];then
        OS_TYPE="ohos"
    elif [ "$HOS_KERNEL_TYPE" = "linux" ];then
        OS_TYPE="linux"
    fi
    if [ "$HOS_BUILD_COMPILER" = "gcc" ];then
        COMPILER="gcc"
    fi
    COMPILER_VER="himix100"
    if [ "$BOARD_NAME" = "hi3516dv300" ];then
        COMPILER_VER="himix200"
    fi
    echo "###$BOARD_NAME:$OS_TYPE:$COMPILER_VER###"
    sed -i "/^CFG_CHIP_TYPE=/cCFG_CHIP_TYPE=$BOARD_NAME" $ROOT_DIR/device/hisilicon/third_party/ffmpeg/cfg.mak
    sed -i "/^CFG_OS_TYPE=/cCFG_OS_TYPE=$OS_TYPE" $ROOT_DIR/device/hisilicon/third_party/ffmpeg/cfg.mak
    sed -i "/^CFG_LINUX_COMPILER_VER=/cCFG_LINUX_COMPILER_VER=$COMPILER_VER" $ROOT_DIR/device/hisilicon/third_party/ffmpeg/cfg.mak
    sed -i "/^CFG_OHOS_BUILD_PATH=/cCFG_OHOS_BUILD_PATH=$OHOS_BUILD_PATH" $ROOT_DIR/device/hisilicon/third_party/ffmpeg/cfg.mak
    sed -i "/^LD=/cLD=$OHOS_BUILD_PATH/clang" $ROOT_DIR/device/hisilicon/third_party/ffmpeg/configure_llvm
    cd $ROOT_DIR/device/hisilicon/third_party/ffmpeg; make clean; make -j; cd -;
    cp -rf $ROOT_DIR/device/hisilicon/third_party/ffmpeg/ffmpeg-y/install/lib/libavcodec.so $OUT_DIR/
    cp -rf $ROOT_DIR/device/hisilicon/third_party/ffmpeg/ffmpeg-y/install/lib/libavformat.so $OUT_DIR/
    cp -rf $ROOT_DIR/device/hisilicon/third_party/ffmpeg/ffmpeg-y/install/lib/libavutil.so $OUT_DIR/
    if [ "$HOS_KERNEL_TYPE" = "liteos_a" ];then
        cp -rf $CUR_DIR/component/fileformat/mp4/lib/$COMPILER/$OS_TYPE/libmp4.so $OUT_DIR/
        cp -rf $CUR_DIR/component/fileformat/common/lib/$COMPILER/$OS_TYPE/libfileformat.so $OUT_DIR/
        cp -rf $CUR_DIR/component/recorder_pro/lib/$COMPILER/$OS_TYPE/librecorder_pro.so $OUT_DIR/
        cp -rf $CUR_DIR/component/dtcf/lib/$COMPILER/$OS_TYPE/libdtcf.so $OUT_DIR/
        cp -rf $CUR_DIR/component/fstool/lib/$COMPILER/$OS_TYPE/libfstool.so $OUT_DIR/
        cp -rf $CUR_DIR/common/hitimer/lib/$COMPILER/$OS_TYPE/libhitimer.so $OUT_DIR/
        cp -rf $CUR_DIR/common/mbuffer/lib/$COMPILER/$OS_TYPE/libmbuf.so $OUT_DIR/
        cp -rf $CUR_DIR/common/log/lib/$COMPILER/$OS_TYPE/libmwlog.so $OUT_DIR/
    elif [ "$HOS_KERNEL_TYPE" = "linux" ];then
        cp -rf $CUR_DIR/component/fileformat/mp4/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libmp4.so $OUT_DIR/
        cp -rf $CUR_DIR/component/fileformat/common/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libfileformat.so $OUT_DIR/
        cp -rf $CUR_DIR/component/recorder_pro/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/librecorder_pro.so $OUT_DIR/
        cp -rf $CUR_DIR/component/dtcf/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libdtcf.so $OUT_DIR/
        cp -rf $CUR_DIR/component/fstool/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libfstool.so $OUT_DIR/
        cp -rf $CUR_DIR/common/hitimer/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libhitimer.so $OUT_DIR/
        cp -rf $CUR_DIR/common/mbuffer/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libmbuf.so $OUT_DIR/
        cp -rf $CUR_DIR/common/log/lib/$COMPILER/$OS_TYPE/$COMPILER_VER/libmwlog.so $OUT_DIR/
    fi

}

main "$@"
