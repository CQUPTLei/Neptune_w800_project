/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_LITE_HOTA_HAL_BOARD_H
#define OHOS_LITE_HOTA_HAL_BOARD_H

#include "hota_partition.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**
 * @brief OTA module initialization.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalInit(void);

/**
 * @brief Release OTA module resource.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalDeInit(void);

/**
 * @brief Release OTA module resource.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalGetUpdateIndex(unsigned int *index);

/**
 * @brief Write image to partition.
 *
 * @param partition [in] scan result, result array size must larger than WIFI_SCAN_AP_LIMIT.
 * @param buffer    [in] image buffer.
 * @param offset    [in] The buffer offset of file.
 * @param bufLen    [in] The Length of buffer.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalWrite(HotaPartition partition, unsigned char *buffer, unsigned int offset, unsigned int bufLen);

/**
 * @brief read image of partition.
 *
 * @param partition [in]  scan result, result array size must larger than WIFI_SCAN_AP_LIMIT.
 * @param offset    [in]  The buffer offset of file.
 * @param bufLen    [in]  The Length of buffer.
 * @param buffer    [out] image buffer.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRead(HotaPartition partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer);

/**
 * @brief Write Boot Settings in order to notify device upgrade success or enter Recovery Part.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalSetBootSettings(void);

/**
 * @brief Restart after upgrade finish or go bootloader to upgrade.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRestart(void);

/**
 * @brief Rollback if ota failed.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRollback(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_HAL_BOARD_H */
