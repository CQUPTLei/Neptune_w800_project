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
CC_PATH="$6"

function main(){
    CUR_DIR=$(cd $(dirname "$0");pwd)
    echo "BOARD_NAME=${BOARD_NAME} HOS_BUILD_COMPILER=${HOS_BUILD_COMPILER} STORAGE_TYPE=${STORAGE_TYPE} HOS_KERNEL_TYPE=${HOS_KERNEL_TYPE} CC_PATH=${CC_PATH}"
    COMPILER_TYPE=gcc
    if [ "$HOS_BUILD_COMPILER" == "clang" ]; then
        COMPILER_TYPE=llvm
    fi
    cd $CUR_DIR/source
    if [ "$HOS_KERNEL_TYPE" == "liteos_a" ]; then
        if [ "$COMPILER_TYPE" == "llvm" ]; then
	    ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE $CC_PATH
        else
	    ./build_another.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE $CC_PATH
        fi
    elif [ "$HOS_KERNEL_TYPE" == "linux" ]; then
        if [ "$STORAGE_TYPE" == "emmc" ]; then
	    ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE $CC_PATH
        else
	    ./build_another.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE $CC_PATH
        fi
    fi
    cd $CUR_DIR
}

main "$@"

