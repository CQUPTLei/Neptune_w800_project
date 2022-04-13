#!/bin/bash

set -e
#make distclean
make lib 
make

#IMG_DIR=${CUR_DIR}/bin/${product}
#OHOS_OUT=${CUR_DIR}/../../../out
#export OHOS_LIB=${CUR_DIR}/bin/build/ohos/lib
#echo "OHOS_OUT == $OHOS_OUT, OHOS_LIB == $OHOS_LIB"

#rm -rf ${OHOS_LIB}
#mkdir -pv ${OHOS_LIB}
#find ${OHOS_OUT}/${product}/libs/ -name '*.a' -exec cp "{}" ${OHOS_LIB} \;

# export USE_DEMO_APP=true

#make lib && make
#exitcode=$?

#if [ $exitcode -eq 0 ]; then
#    echo "Build success!"
#    if [ ! -d $OHOS_OUT ]; then
#        mkdir $OHOS_OUT
#    fi
#    cp $IMG_DIR/* $OHOS_OUT/${product}/ -rf
#else
#    echo "Build failed!"
#fi

#exit $exitcode
