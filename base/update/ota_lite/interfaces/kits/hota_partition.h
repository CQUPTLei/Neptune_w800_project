/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_LITE_HOTA_PARTITION_H
#define OHOS_LITE_HOTA_PARTITION_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define PARTITION_NAME_LENGTH   16

typedef enum {
    PARTITION_ERROR = -1,
    PARTITION_PASS_THROUGH = 0,
    PARTITION_INFO_COMP = 1,  /* The info component of ota package. */
    PARTITION_BOOTLOADER = 2,
    PARTITION_KERNEL_A = 3,
    PARTITION_KERNEL_B = 4,
    PARTITION_ROOTFS = 5,
    PARTITION_APP = 6,
    PARTITION_DATA = 7,
    PARTITION_OTA_TAG = 8,
    PARTITION_OTA_CONFIG = 9,
    PARTITION_ROOTFS_EXT4 = 10,
    PARTITION_MAX
} HotaPartition;

typedef struct {
    char partitionName[PARTITION_NAME_LENGTH];
    HotaPartition partition;
} HotaPartitionDef;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_PARTITION_H */
