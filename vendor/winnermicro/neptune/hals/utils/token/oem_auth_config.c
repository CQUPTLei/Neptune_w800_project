/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implement of oem_auth_config
 * Author: Kit Framework group
 * Create: 2020-12-26
 */

#include "oem_auth_config.h"

//no server info,use default server info
#define USE_DEFAULT_SERVER_INF  1

char kitInfo[] = "1.0.0.301";

int OEMReadAuthServerInfo(char* buff, unsigned int len)
{
#ifdef USE_DEFAULT_SERVER_INF
	(void)buff;
	(void)len;
    return -1;
#else
	//copy server info to buff.
#endif
}

char* OEMLoadKitInfos(void)
{
    return kitInfo;
}
