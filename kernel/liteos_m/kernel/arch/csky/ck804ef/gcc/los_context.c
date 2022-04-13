/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
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

#include "los_context.h"
#include "los_compiler.h"
#include "los_typedef.h"
#include "csi_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


extern UINT32 g_vuwIntCount ;
extern UINT32 XPORT_LOS_IntLock(void);
extern void LOS_GetSystickCycle(UINT32 *cntHi, UINT32 *cntLo);
extern VOID *XPORT_OsTskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack);

UINTPTR HalIntLock(VOID)
{
    return XPORT_LOS_IntLock();
}

UINT32 HalIsIntAcvive(VOID)
{
    return (UINT32)g_vuwIntCount > 0;
}

LITE_OS_SEC_TEXT_MINOR VOID HalGetCpuCycle(UINT32 *cntHi, UINT32 *cntLo)
{
    LOS_GetCpuCycle(cntHi, cntLo);
    return;
}

VOID HalIntRestore(UINTPTR intSave)
{
    XPORT_LOS_IntRestore(intSave);
}

WEAK VOID HalDelay(UINT32 ticks)
{

}
void HAL_NVIC_SystemReset(void)
{
    NVIC_SystemReset();
}
LITE_OS_SEC_TEXT VOID HalTaskScheduleCheck(VOID)
{

}

LITE_OS_SEC_TEXT VOID HalTaskSchedule(VOID)
{
    //XPORT_LOS_Schedule();
    asmOSTaskSchedule();
}
#ifndef errno_t
typedef int errno_t;
#endif

void __attribute__((weak)) OHOS_SystemInit(void)
{
    return;
}



/* ****************************************************************************
 Function    : HalArchInit
 Description : arch init function
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
LITE_OS_SEC_TEXT_INIT VOID HalArchInit()
{
    //HalHwiInit();
}

/* ****************************************************************************
 Function    : HalSysExit
 Description : Task exit function
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
LITE_OS_SEC_TEXT_MINOR VOID HalSysExit(VOID)
{
    //LOS_IntLock();
    while (1) {
    }
}

/* ****************************************************************************
 Function    : HalTskStackInit
 Description : Task stack initialization function
 Input       : taskID     --- TaskID
               stackSize  --- Total size of the stack
               topStack    --- Top of task's stack
 Output      : None
 Return      : Context pointer
 **************************************************************************** */
LITE_OS_SEC_TEXT_INIT VOID *HalTskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack)
{
    XPORT_OsTskStackInit(taskID, stackSize, topStack);
}

LITE_OS_SEC_TEXT_INIT UINT32 HalStartSchedule(OS_TICK_HANDLER handler)
{
    UINT32 ret;
    ret = OsTickStart(handler);
    if (ret != LOS_OK) {
        return ret;
    }
    XPORT_LOS_StartToRun();

    
    return LOS_OK; /* never return */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
