#!/bin/bash

#Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

# This script is used to prepare header files for musl's libc.so.

set -e

PRJ_ROOT="$PWD/../../../../"
KERNEL_ROOT="$PRJ_ROOT/third_party/Linux_Kernel"
ARM_HDR="$PRJ_ROOT/third_party/Linux_Kernel/hdr_install/arm_header"
OUT_HDR="$PRJ_ROOT/prebuilts/lite/sysroot/thirdparty/linux_headers_install"
export Z_BUILD_TOP="$PRJ_ROOT"

if [ -d "$ARM_HDR" ];then
rm -r $ARM_HDR
fi

if [ -d "$OUT_HDR" ];then
rm -r $OUT_HDR
fi

mkdir -p $ARM_HDR
mkdir -p $OUT_HDR

pushd $KERNEL_ROOT
make headers_install ARCH=arm INSTALL_HDR_PATH=$ARM_HDR
popd

cp -r $KERNEL_ROOT/include/uapi/* $OUT_HDR
cp -r $ARM_HDR/include/asm  $OUT_HDR
