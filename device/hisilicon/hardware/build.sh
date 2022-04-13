#!/bin/bash
# Copyright 2020-2020, Huawei Technologies Co. Ltd.
#
# ALL RIGHTS RESERVED
#
# Compile mpp/sample project, this is the entrance script

# error out on errors
set -e
OUT_DIR="$1"
BOARD_NAME="$2"
HOS_KERNEL_TYPE="$3"
HOS_BUILD_COMPILER="$4"
STORAGE_TYPE="$5"

function main(){
    CUR_DIR=$(cd $(dirname "$0");pwd)

    COMPILER_TYPE=$HOS_BUILD_COMPILER
    if [ "$HOS_BUILD_COMPILER" == "clang" ]; then
        COMPILER_TYPE=llvm
    fi
    cd $CUR_DIR/media
    if [ "$HOS_KERNEL_TYPE" == "liteos_a" ]; then
        if [ "$COMPILER_TYPE" == "llvm" ]; then
            ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE
        else
            ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE
        fi
    elif [ "$HOS_KERNEL_TYPE" == "linux" ]; then
        if [ "$STORAGE_TYPE" == "emmc" ]; then
            ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE
        else
            ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE
        fi
    fi
    cd $CUR_DIR
}

main "$@"
