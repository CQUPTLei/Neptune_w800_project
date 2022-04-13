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
# Compile mpp/sample project, this is the entrance script

# error out on errors
set -e
OUT_DIR="$1"
DEVICE_PATH="$2"
BOARD="$3"
OHOS_BUILD_COMPILER="$5"
function copySocLib() {
    ROOT_DIR=$(cd $(dirname "$0");pwd)
    SOURCE_LIB="$ROOT_DIR/../../../../device/hisilicon/hispark_aries/sdk_liteos/mpp/lib"
    if [ "$OHOS_BUILD_COMPILER" = "gcc" ];then
        SOURCE_LIB="$ROOT_DIR/../../../../device/hisilicon/hi3518ev300/sdk_liteos/mpp/lib"
#        cp -rf ./libsns_imx307_2l.so $OUT_DIR/
    fi

    cp -rf $SOURCE_LIB/./libmpi.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hiacs.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hiir_auto.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hiawb.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libive.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libdnvqe.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hidrc.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hildci.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libupvqe.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hidehaze.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hicalcflicker.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libVoiceEngine.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libisp.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libsns_imx307.so $OUT_DIR/
#    cp -rf $SOURCE_LIB/./libsns_imx307_2l.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libsns_f23.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./lib_hiae.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libaacdec.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libaacenc.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libaaccomm.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libaacsbrdec.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libaacsbrenc.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libsecurec.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libtde.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_RES.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_record.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_HPF.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_ANR.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_common.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_AEC.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_AGC.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libhive_EQ.so $OUT_DIR/
    cp -rf $SOURCE_LIB/./libivp.so $OUT_DIR/
}
function main(){
    ROOT_DIR=$(cd $(dirname "$0");pwd)
    copySocLib
    cd "$ROOT_DIR"
    make clean && make -j16 OUTDIR=$OUT_DIR
    cp OHOS_Image* $OUT_DIR/
}
main "$@"
