/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hota_verify.h"

#include <securec.h>
#include "app_rsa.h"
#include "app_sha256.h"

#define PUBKEY_LENGTH 270

static uint8 g_pubKeyBuf[PUBKEY_LENGTH] = {
    0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xBF, 0xAA, 0xA5, 0xB3, 0xC2, 0x78, 0x5E,
    0x63, 0x07, 0x84, 0xCF, 0x37, 0xF0, 0x45, 0xE8, 0xB9, 0x6E, 0xEF, 0x04, 0x88, 0xD3, 0x43, 0x06,
    0x2C, 0xFC, 0x09, 0x8C, 0x84, 0x3B, 0x43, 0x07, 0x89, 0x6B, 0x23, 0x8F, 0xA2, 0xF0, 0x9D, 0x91,
    0xCB, 0xA8, 0x02, 0x60, 0xD8, 0x5C, 0x18, 0xD9, 0x34, 0xF0, 0x3C, 0x49, 0x10, 0x0E, 0xE3, 0xC7,
    0x19, 0xA5, 0x51, 0x93, 0x38, 0xFD, 0xE0, 0x62, 0x64, 0xBA, 0x6D, 0x11, 0x2E, 0xE1, 0x6E, 0x60,
    0x12, 0x16, 0x1B, 0x35, 0xA6, 0x54, 0x3F, 0x0B, 0x5D, 0x54, 0x08, 0xC9, 0x23, 0x51, 0x15, 0xA9,
    0xE2, 0x07, 0xCC, 0xF9, 0xFD, 0x19, 0x8A, 0xB3, 0x7E, 0xCE, 0x69, 0xED, 0x26, 0x34, 0xD2, 0x02,
    0xF3, 0xEB, 0x07, 0x13, 0x69, 0xE3, 0x03, 0x87, 0xB3, 0x6A, 0x3E, 0x91, 0x94, 0xAC, 0x2C, 0xBA,
    0xF6, 0xEE, 0x4C, 0x41, 0x0C, 0x2E, 0xD0, 0xE7, 0x58, 0xA7, 0xE6, 0x7F, 0x1A, 0xC0, 0xB8, 0xE3,
    0x12, 0x18, 0x97, 0x8D, 0x99, 0xAB, 0x35, 0x7B, 0xAD, 0x41, 0xA0, 0xFB, 0xCB, 0x23, 0xF6, 0x51,
    0xE6, 0x94, 0x1F, 0xF7, 0xD8, 0x16, 0xCD, 0x15, 0x67, 0x59, 0x10, 0xA2, 0x40, 0x55, 0xD1, 0x67,
    0xDA, 0x18, 0xCD, 0x63, 0x5B, 0x10, 0xAF, 0x22, 0x99, 0xD8, 0x9D, 0x56, 0x66, 0xCD, 0x80, 0x64,
    0x32, 0xB5, 0xD7, 0xF5, 0xBA, 0x91, 0x4A, 0x8D, 0x97, 0x14, 0x8A, 0xB2, 0xB0, 0x42, 0x4A, 0xE6,
    0x43, 0x22, 0x3B, 0x6E, 0xD6, 0x1E, 0x1F, 0xDA, 0xEC, 0x83, 0xCF, 0x20, 0xFA, 0x02, 0xF3, 0xFB,
    0x6E, 0x09, 0x2A, 0x0D, 0xB7, 0x81, 0x1E, 0xD5, 0x71, 0xDF, 0x80, 0xC5, 0x33, 0x78, 0xE5, 0x41,
    0x33, 0xDF, 0x9A, 0xBD, 0x36, 0x51, 0xAC, 0x96, 0xF4, 0xC6, 0x11, 0xC3, 0x93, 0x78, 0x26, 0x96,
    0x9F, 0x67, 0x05, 0x1D, 0xDF, 0xB3, 0xAA, 0x26, 0x25, 0x02, 0x03, 0x01, 0x00, 0x01
};

static uint8 g_hash[HASH_LENGTH] = {0};

static AppSha256Context g_sha256 = { 0 }; /* hash tmp var */

void HotaHashInit(void)
{
    AppSha256Init(&g_sha256);
    // Init the hash value of data
    if (memset_s(g_hash, HASH_LENGTH, 0, HASH_LENGTH) != EOK) {
        return;
    }

    return;
}

void HotaHashCalc(const uint8 *buffer, uint32 length)
{
    if (buffer == NULL) {
        printf("HashCalc param in null!\r\n");
        return;
    }

    AppSha256Update(&g_sha256, buffer, length);
    return;
}

int32 HotaGetHash(uint8 *buffer, uint32 length)
{
    if (buffer == NULL) {
        printf("GetHash param in null!\r\n");
        return OHOS_FAILURE;
    }

    if (memset_s(buffer, length, 0, length) != EOK) {
        return OHOS_FAILURE;
    }

    if (length >= HASH_LENGTH) {
        // Calc the last hash, and save it
        AppSha256Finish(&g_sha256, g_hash);
        if (memcpy_s(buffer, HASH_LENGTH, g_hash, HASH_LENGTH) != EOK) {
            return OHOS_FAILURE;
        }
        return OHOS_SUCCESS;
    } else {
        printf("GetHash len illegal!\r\n");
        return OHOS_FAILURE;
    }
}

static int32 HotaCalcImageHash(uint8 *dataAddr, uint32 dataLen, uint8 *hash, uint32 hashLen)
{
    AppSha256Context sha256;
    uint32 count;

    if ((dataAddr == NULL) || (hash == NULL) || (dataLen == 0) || (hashLen < HASH_LENGTH)) {
        printf("HotaCalcImageHash param is illegal.\r\n");
        return OHOS_FAILURE;
    }

    uint8 *databuf = dataAddr;
    if (memset_s(hash, hashLen, 0, hashLen) != EOK) {
        printf("HotaCalcImageHash memset_s hash failed.\r\n");
        return OHOS_FAILURE;
    }
    AppSha256Init(&sha256);

    while (dataLen > 0) {
        count = (dataLen > BUFFR_LENGTH) ? BUFFR_LENGTH : dataLen;
        AppSha256Update(&sha256, databuf, count);
        dataLen -= count;
        databuf += count;
    }
    // Calc Final hash
    AppSha256Finish(&sha256, hash);

    return OHOS_SUCCESS;
}

static int32 HotaSignVerifyByHash(const uint8 *hash, uint32 hashLen, const uint8 *imageSign, uint32 signLen)
{
    AppRsaContext rsa;
    uint32 length = 0;
    if ((hash == NULL) || (imageSign == NULL)) {
        return OHOS_FAILURE;
    }

    AppRsaInit(&rsa);
    uint8 *keyBuf = HotaGetPubKey(&length);
    if (keyBuf == NULL) {
        return OHOS_FAILURE;
    }

    int ret = AppRsaDecodePublicKey(&rsa, keyBuf, length);
    if (ret != 0) {
        return OHOS_FAILURE;
    }

    ret = AppVerifyData(&rsa, hash, hashLen, imageSign, signLen);
    AppRsaFree(&rsa);
    if (ret < 0) {
        printf("App verify failed.\r\n");
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int32 HotaSignVerify(uint8 *image, uint32 imgLen, uint8 *imageSign, uint32 signLen)
{
    if ((image == NULL) || (imageSign == NULL)) {
        printf("SignVerify param in illegal, len %u!\r\n", signLen);
        return OHOS_FAILURE;
    }

    uint8 imageHashLocal[HASH_LENGTH] = {0};
    if (HotaCalcImageHash(image, imgLen, imageHashLocal, HASH_LENGTH) != OHOS_SUCCESS) {
        printf("HotaCalcImageHash fail!\r\n");
        return OHOS_FAILURE;
    }

    // Use local hash of data to verify the sign
    int ret = HotaSignVerifyByHash(imageHashLocal, HASH_LENGTH, imageSign, SIGN_DATA_LEN);
    if (ret != OHOS_SUCCESS) {
        printf("SignVerify fail!\r\n");
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

uint8 *HotaGetPubKey(uint32 *length)
{
    if (length == NULL) {
        printf("OTA pub key is NULL.\r\n");
        return NULL;
    }

    *length = sizeof(g_pubKeyBuf);
    return (uint8 *)g_pubKeyBuf;
}
