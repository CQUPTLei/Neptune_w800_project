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

PRJ_ROOT="${PWD}/../../../../"
LINUX_HDR="${PRJ_ROOT}/prebuilts/lite/sysroot/thirdparty/linux_headers_install"
TMP_DIR_ORI="${PRJ_ROOT}/prebuilts/lite/sysroot/ohos_tmp_ori"
TMP_DIR="${PRJ_ROOT}/prebuilts/lite/sysroot/ohos_tmp"
if [ $# -eq 1 ]; then
	MUSL_DIR="$1/include"
else
	MUSL_DIR="${PWD}/musl/include"
fi
export Z_BUILD_TOP="${PRJ_ROOT}"

if [ ! -d "${LINUX_HDR}/asm" ] || [ ! -d "${LINUX_HDR}/asm-generic" ] || [ ! -d "${LINUX_HDR}/linux" ];then
    echo "please install headers first!!!"
    echo "see guide at ${SYSROOT}/../../thirdparty/README"
    exit 1
fi

mkdir -p ${TMP_DIR_ORI}
mkdir -p ${TMP_DIR}

mkdir -p ${TMP_DIR_ORI}/uapi/asm-generic
mkdir -p ${TMP_DIR_ORI}/uapi/asm
mkdir -p ${TMP_DIR_ORI}/uapi/linux

cp ${LINUX_HDR}/asm-generic/bitsperlong.h ${TMP_DIR_ORI}/uapi/asm-generic
cp ${LINUX_HDR}/asm-generic/int-ll64.h ${TMP_DIR_ORI}/uapi/asm-generic
cp ${LINUX_HDR}/asm-generic/posix_types.h ${TMP_DIR_ORI}/uapi/asm-generic
cp ${LINUX_HDR}/asm/bitsperlong.h ${TMP_DIR_ORI}/uapi/asm
cp ${LINUX_HDR}/asm/posix_types.h ${TMP_DIR_ORI}/uapi/asm
cp ${LINUX_HDR}/asm/types.h ${TMP_DIR_ORI}/uapi/asm
cp ${LINUX_HDR}/linux/capability.h ${TMP_DIR_ORI}/uapi/linux
cp ${LINUX_HDR}/linux/posix_types.h ${TMP_DIR_ORI}/uapi/linux
cp ${LINUX_HDR}/linux/stddef.h ${TMP_DIR_ORI}/uapi/linux
cp ${LINUX_HDR}/linux/types.h ${TMP_DIR_ORI}/uapi/linux

echo "#ifndef _UAPI_LINUX_COMPILER_H" >> ${TMP_DIR_ORI}/uapi/linux/compiler.h
echo "#define _UAPI_LINUX_COMPILER_H" >> ${TMP_DIR_ORI}/uapi/linux/compiler.h
echo "#define __user" >> ${TMP_DIR_ORI}/uapi/linux/compiler.h
echo "#endif" >> ${TMP_DIR_ORI}/uapi/linux/compiler.h

pushd ${PRJ_ROOT}
python prebuilts/lite/sysroot/build/update_headers.py 2>/dev/null
popd

cp -rf  ${TMP_DIR}/uapi/* ${MUSL_DIR}/
rm -rf ${TMP_DIR_ORI}
rm -rf ${TMP_DIR}
