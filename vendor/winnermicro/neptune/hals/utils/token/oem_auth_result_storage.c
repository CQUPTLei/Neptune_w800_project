/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implement of oem_auth_result_storage
 * Author: Kit Framework group
 * Create: 2020-12-26
 */

#include <limits.h>
#include <stdio.h>
#include "wm_internal_flash.h"
#include "wm_flash_map.h"
#include "oem_auth_result_storage.h"


//0x81E0200 ~ 0x81E03FF
#define SAVE_RESET_FLAG_ADDR	(USER_ADDR_START + 0x1A0)		//len 32
#define SAVE_STATUS_FLAG_ADDR	(USER_ADDR_START + 0x1C0)       //len 32 : flag (4) + len (4)
#define SAVE_TICKET_FLAG_ADDR	(USER_ADDR_START + 0x1E0)		//len 32 : flag (4) + len (4)
#define SAVE_TICKET_MEM_ADDR	(USER_ADDR_START + 0x200)		//len 512
#define SAVE_STATUS_MEM_ADDR	(USER_ADDR_START + 0x400)       //len 5K

#define RESET_FLAG 				0x5A5A5A5A
#define STATUS_FLAG 			0xA5A5A5A5
#define TICK_FLAG 				0xC0C0C0C0

bool OEMIsOverTemperatureLimit(void)
{
    // neptune not check temprature limit yet.
    return false;
}

bool OEMIsResetFlagExist(void)
{
    int ret;
	unsigned int flag;

    ret = tls_fls_read(SAVE_RESET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if ((ret != TLS_FLS_STATUS_OK) || (flag != RESET_FLAG))
		return -1;

	return 0;
}

int OEMCreateResetFlag(void)
{
    int ret;
	unsigned int flag = RESET_FLAG;

	ret = tls_fls_write(SAVE_RESET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return ret;
}

int OEMDeleteResetFlag(void)
{
    int ret;
	unsigned int flag = 0;

	ret = tls_fls_write(SAVE_RESET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return ret;

}

bool OEMIsAuthStatusExist(void)
{
    int ret;
	unsigned int flag;

    ret = tls_fls_read(SAVE_STATUS_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if ((ret != TLS_FLS_STATUS_OK) || (flag != STATUS_FLAG))
		return -1;

	return 0;
}

int OEMWriteAuthStatus(const char* data, unsigned int len)
{
    int ret;
	unsigned int flag = STATUS_FLAG;

	ret = tls_fls_write(SAVE_STATUS_MEM_ADDR, data, len);
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_STATUS_FLAG_ADDR + 0x4, (unsigned char *)&len, sizeof(len));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_STATUS_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return ret;
}

int OEMReadAuthStatus(char* buffer, unsigned int bufferLen)
{
    int ret;
	unsigned int flag;

    ret = tls_fls_read(SAVE_STATUS_MEM_ADDR, buffer, bufferLen);
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	return 0;
}

int OEMDeleteAuthStatus(void)
{
	int ret;
	unsigned int flag = 0;

	ret = tls_fls_write(SAVE_STATUS_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_STATUS_FLAG_ADDR + 0x4, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return 0;
}

int OEMGetAuthStatusFileSize(unsigned int* len)
{
    int ret;

    ret = tls_fls_read(SAVE_STATUS_FLAG_ADDR + 0x4, (unsigned char *)len, sizeof(unsigned int));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	return 0;
}

bool OEMIsTicketExist(void)
{
    int ret;
	unsigned int flag;

    ret = tls_fls_read(SAVE_TICKET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if ((ret != TLS_FLS_STATUS_OK) || (flag != TICK_FLAG))
		return -1;

	return 0;
}

int OEMWriteTicket(const char* data, unsigned int len)
{
    int ret;
	unsigned int flag = TICK_FLAG;

	ret = tls_fls_write(SAVE_TICKET_MEM_ADDR, data, len);
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_TICKET_FLAG_ADDR + 0x4, (unsigned char *)&len, sizeof(unsigned int));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_TICKET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return ret;
}

int OEMReadTicket(char* buffer, unsigned int bufferLen)
{
    int ret;
	unsigned int flag;

    ret = tls_fls_read(SAVE_TICKET_MEM_ADDR, buffer, bufferLen);
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	return 0;
}

int OEMDeleteTicket(void)
{
	int ret;
	unsigned int flag = 0;

	ret = tls_fls_write(SAVE_TICKET_FLAG_ADDR, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	ret = tls_fls_write(SAVE_TICKET_FLAG_ADDR + 0x4, (unsigned char *)&flag, sizeof(flag));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

    return 0;
}

int OEMGetTicketFileSize(unsigned int* len)
{
    int ret;

    ret = tls_fls_read(SAVE_TICKET_FLAG_ADDR + 0x4, (unsigned char *)len, sizeof(unsigned int));
	if (ret != TLS_FLS_STATUS_OK)
		return -1;

	return 0;
}
