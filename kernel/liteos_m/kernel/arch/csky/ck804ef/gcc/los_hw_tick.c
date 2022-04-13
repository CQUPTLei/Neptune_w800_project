/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// #include "los_tick_pri.h"
// #include "los_base.h"
// #include "los_task_pri.h"
// #include "los_swtmr.h"
// #include "los_hwi.h"
#include "los_tick.h"
#include "los_config.h"
#include "los_timer.h"

#include "csi_core.h"
#include "wm_regs.h"

#define CYCLE_CHECK                          0xFFFFFFFFU
#define SHIFT_32_BIT                         32

/**W800 BASE PLL CLOCK*/
#define W800_PLL_CLK_MHZ  		(480)
#define UNIT_MHZ				(1000000)

enum CPU_CLK{
	CPU_CLK_240M = 2,
	CPU_CLK_160M = 3,
	CPU_CLK_80M  = 6,
	CPU_CLK_40M  = 12,
	CPU_CLK_2M  = 240,
};


//extern unsigned int csi_coret_get_value(void);

/* ****************************************************************************
Function    : OsTickStart
Description : Configure Tick Interrupt Start
Input       : none
output      : none
return      : LOS_OK - Success , or LOS_ERRNO_TICK_CFG_INVALID - failed
**************************************************************************** */
LITE_OS_SEC_TEXT_INIT UINT32 OsTickStart(VOID)
{
    UINT32 ret;

    if ((OS_SYS_CLOCK == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND > OS_SYS_CLOCK)) {
        return LOS_ERRNO_TICK_CFG_INVALID;
    }

	//CPU_CLK_80M should same as tls_sys_clk_set()
    g_cyclesPerTick = W800_PLL_CLK_MHZ*UNIT_MHZ / CPU_CLK_80M / LOSCFG_BASE_CORE_TICK_PER_SECOND;
    g_ullTickCount = 0;

	//tls_sys_clk_set(CPU_CLK_80M); //init in wm_main.c

    return LOS_OK;
}

/* ****************************************************************************
Function    : LOS_SysTickCurrCycleGet
Description : Get System cycle count
Input       : none
output      : none
return      : hwCycle --- the system cycle count
**************************************************************************** */
LITE_OS_SEC_TEXT_MINOR UINT32 LOS_SysTickCurrCycleGet(VOID)
{
    UINT32 hwCycle = 0;
    UINTPTR intSave;

    intSave = XPORT_LOS_IntLock();
    hwCycle = csi_coret_get_value();

    /* tick int has come, to do +1 */
    if (NVIC_GetActive(SYS_TICK_IRQn)) {
        hwCycle = csi_coret_get_value();
        hwCycle += g_cyclesPerTick;
    }

    XPORT_LOS_IntRestore(intSave);

    return hwCycle;
}

/* ****************************************************************************
Function    : LOS_GetCpuCycle
Description : Get System cycle count
Input       : none
output      : cntHi  --- CpuTick High 4 byte
              cntLo  --- CpuTick Low 4 byte
return      : none
**************************************************************************** */
void LOS_GetCpuCycle(UINT32 *cntHi, UINT32 *cntLo)
{
    UINT64 swTick;
    UINT64 cycle;
    UINT32 hwCycle;
    UINTPTR intSave;

    intSave = XPORT_LOS_IntLock();

    swTick = g_ullTickCount;
    hwCycle = csi_coret_get_value();

    /* tick int has come, to do +1 */
    if (NVIC_GetActive(SYS_TICK_IRQn)) {
        hwCycle = csi_coret_get_value();
        swTick++;
    }

    cycle = (((swTick) * g_cyclesPerTick) + (g_cyclesPerTick - hwCycle));

    *cntHi = cycle >> SHIFT_32_BIT;
    *cntLo = cycle & CYCLE_CHECK;

    XPORT_LOS_IntRestore(intSave);

    return;
}

/* ****************************************************************************
Function    : LOS_GetSystickCycle
Description : Get Sys tick cycle count
Input       : none
output      : cntHi  --- SysTick count High 4 byte
              cntLo  --- SysTick count Low 4 byte
return      : none
**************************************************************************** */
void LOS_GetSystickCycle(UINT32 *cntHi, UINT32 *cntLo)
{
    UINT64 swTick;
    UINT64 cycle;
    UINT32 hwCycle;
    UINTPTR intSave;
    UINT32 systickLoad;
    UINT32 systickCur;

    intSave = XPORT_LOS_IntLock();

    swTick = g_ullTickCount;

    systickLoad = csi_coret_get_load();
    systickCur = csi_coret_get_value();
    if (systickLoad < systickCur) {
        XPORT_LOS_IntRestore(intSave);
        return;
    }
    hwCycle = systickLoad - systickCur;

    /* tick int has come, to do +1 */
    if (NVIC_GetActive(SYS_TICK_IRQn)) {
        hwCycle = systickLoad - systickCur;
        swTick++;
    }

    cycle = hwCycle + swTick * systickLoad;
    *cntHi = cycle >> SHIFT_32_BIT;
    *cntLo = cycle & CYCLE_CHECK;

    XPORT_LOS_IntRestore(intSave);

    return;
}
