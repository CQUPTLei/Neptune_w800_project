/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.	
 */

#include "hal_token.h"
#include "wm_internal_flash.h"
#include "wm_flash_map.h"
#include "ohos_errno.h"

// 0x81E0000 ~ 0x81E01FF
#define SAVE_TOKEN_MEM_ADDR_A 		(USER_ADDR_START)                //len 192
#define SAVE_TOKEN_MEM_ADDR_B 		(USER_ADDR_START + 0xC0)	     //len 192
#define SAVE_TOKEN_MEM_ADDR_FLAG	(USER_ADDR_START + 2 * 0xC0)     //len 32

#define TOKEN_DATA_LEN 151

static int OEMReadFlag(bool* flag, unsigned int flagLen)
{
    if (flag == NULL) {
        return -1;
    }

    int ret = tls_fls_read(SAVE_TOKEN_MEM_ADDR_FLAG, flag, flagLen);
	if (ret != TLS_FLS_STATUS_OK)
		return EC_FAILURE;

	return EC_SUCCESS;
}

static int OEMWriteFlag(bool flag, unsigned int flagLen)
{
	bool localFlag = flag;

    int ret = tls_fls_write(SAVE_TOKEN_MEM_ADDR_FLAG, &localFlag, flagLen);
	if (ret != TLS_FLS_STATUS_OK)
		return EC_FAILURE;

	return EC_SUCCESS;
}

static int OEMReadToken(char *token, unsigned int len)
{
    bool flag = false;
    unsigned int readTokenAddr;
	int ret;

    if (len < TOKEN_DATA_LEN) {
        return EC_NOMEMORY;
    }
    if (token == NULL) {
        return EC_BADPTR;
    }

    ret = OEMReadFlag(&flag, sizeof(flag));
    if (ret != EC_SUCCESS) {
        return EC_BADPTR;
    }
	
    if (flag) {
        readTokenAddr = SAVE_TOKEN_MEM_ADDR_A;
    } else {
        readTokenAddr = SAVE_TOKEN_MEM_ADDR_B;
    }

    ret = tls_fls_read(readTokenAddr, token, len);
	if (ret != TLS_FLS_STATUS_OK)
		return EC_FAILURE;

	return EC_SUCCESS;
}

static int OEMWriteToken(const char *token, unsigned int len)
{
    if (len < TOKEN_DATA_LEN) {
        return EC_NOMEMORY;
    }
    if (token == NULL) {
        return EC_BADPTR;
    }
	
    bool flag = false;
    int ret = OEMReadFlag(&flag, sizeof(flag));
    if (ret != EC_SUCCESS) {
        return EC_BADPTR;
    }

    unsigned int writeToeknAddr;
    if (flag) {
        writeToeknAddr = SAVE_TOKEN_MEM_ADDR_B;
    } else {
        writeToeknAddr = SAVE_TOKEN_MEM_ADDR_A;
    }

    int writeRet = tls_fls_write(writeToeknAddr, token, len);
    int writeFlagRet;
    if (writeRet == TLS_FLS_STATUS_OK) {
        writeFlagRet = OEMWriteFlag(!flag, sizeof(flag));
        if (writeFlagRet != EC_SUCCESS) {
            return OEMWriteFlag(!flag, sizeof(flag));
        }
        return EC_SUCCESS;
    }

    writeRet = tls_fls_write(writeToeknAddr, token, len);
    if (writeRet != TLS_FLS_STATUS_OK) {
        return writeRet;
    }
    writeFlagRet = OEMWriteFlag(!flag, sizeof(flag));
    if (writeFlagRet != EC_SUCCESS) {
        return OEMWriteFlag(!flag, sizeof(flag));
    }
    return EC_SUCCESS;
}

static int OEMGetAcKey(char *acKey, unsigned int len)
{
    // OEM need add here, get AcKey
    if (acKey == NULL) {
        return EC_BADPTR;
    }
	
    char acKeyBuf[] = {
		0x71,0x30,0x4F,0x63, 0x50,0x2F,0x61,0x73, 0x23,0x26,0x57,0x4A, 0x3A,0x24,0x7A,0x37,
		0xD0,0x1E,0x83,0x85, 0xC0,0x14,0x93,0x21, 0x5E,0xE6,0x7A,0xAB, 0x5D,0xC5,0xE1,0x81,
		0x7D,0x9D,0xD6,0x99, 0xC5,0x18,0xD2,0x45, 0xFE,0x92,0xBD,0x02, 0x61,0x36,0x4F,0xFF,
		0x00};
		
    int bufLen = strlen(acKeyBuf);
    if (len < bufLen) {
        return EC_FAILURE;
    }

	acKey[len - 1] = '\0';
    int ret = memcpy_s(acKey, len, acKeyBuf, bufLen);
    if (ret != 0) {
        return EC_NOMEMORY;
    }
	
    return EC_SUCCESS;
}

static int OEMGetProdId(char *productId, unsigned int len)
{
    // OEM need add here, get ProdId
    if (productId == NULL) {
        return EC_BADPTR;
    }
	
    char productIdBuf[] = "J13W";
    int bufLen = sizeof(productIdBuf) - 1;
    if (len < bufLen) {
        return EC_FAILURE;
    }

    (void)memset_s(productId, len, 0, len);
    int ret = memcpy_s(productId, len, productIdBuf, bufLen);

    if (ret != 0) {
        return EC_NOMEMORY;
    }
	
    return EC_SUCCESS;
}

static int OEMGetProdKey(char *productKey, unsigned int len)
{
   // OEM need add here, get ProdKey
    (void)(productKey);
    (void)(len);
    return EC_SUCCESS;
}

int HalReadToken(char *token, unsigned int len)
{
    if (token == NULL) {
        return EC_FAILURE;
    }

    return OEMReadToken(token, len);
}

int HalWriteToken(const char *token, unsigned int len)
{
    if (token == NULL) {
        return EC_FAILURE;
    }

    return OEMWriteToken(token, len);
}

int HalGetAcKey(char *acKey, unsigned int len)
{
    if (acKey == NULL) {
        return EC_FAILURE;
    }

    return OEMGetAcKey(acKey, len);
}

int HalGetProdId(char *productId, unsigned int len)
{
    if (productId == NULL) {
        return EC_FAILURE;
    }

    return OEMGetProdId(productId, len);
}

int HalGetProdKey(char *productKey, unsigned int len)
{
    if (productKey == NULL) {
        return EC_FAILURE;
    }

    return OEMGetProdKey(productKey, len);
}
