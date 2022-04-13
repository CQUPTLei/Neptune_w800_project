/*
 * Copyright (c) 2013-2019, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020, Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if(LOSCFG_PLATFORM_HWI == YES) 

UINT32 g_vuwIntCount = 0;

/* ****************************************************************************
 Function    : OsHwiInit
 Description : initialization of the hardware interrupt
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
VOID OsHwiInit()
{


    return;
}
/* ****************************************************************************
 Function    : LOS_HwiCreate
 Description : create hardware interrupt
 Input       : hwiNum   --- hwi num to create
               hwiPrio  --- priority of the hwi
               mode     --- unused
               handler --- hwi handler
               arg      --- param of the hwi handler
 Output      : None
 Return      : LOS_OK on success or error code on failure
 **************************************************************************** */
UINT32 LOS_HwiCreate(HWI_HANDLE_T hwiNum,
                                           HWI_PRIOR_T hwiPrio,
                                           HWI_MODE_T mode,
                                           HWI_PROC_FUNC handler,
                                           HWI_ARG_T arg)
{

    return LOS_OK;
}

/* ****************************************************************************
 Function    : LOS_HwiDelete
 Description : Delete hardware interrupt
 Input       : hwiNum   --- hwi num to delete
 Output      : None
 Return      : LOS_OK on success or error code on failure
 **************************************************************************** */
UINT32 LOS_HwiDelete(HWI_HANDLE_T hwiNum)
{

    return LOS_OK;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


