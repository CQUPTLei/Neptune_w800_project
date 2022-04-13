/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#include <stdio.h>
// #include "los_hw.h"
#include "los_task_pri.h"
// #include "los_priqueue_pri.h"
// #include <los_config.h>

#include "los_tick.h"
#include "los_config.h"
#include "los_timer.h"


typedef void (*TaskFunction_t)( void * );
///< here we get a memory for the task 
UINT8 g_sysMemAddrStart[OS_SYS_MEM_SIZE] __attribute__((aligned(32)));


void LOS_TaskTCBUpdate()
{
    if(NULL != g_losTask.runTask )
    {
        g_losTask.runTask->taskStatus &= (~OS_TASK_STATUS_RUNNING);
    }
    g_losTask.runTask = g_losTask.newTask;
    g_losTask.newTask->taskStatus |= OS_TASK_STATUS_RUNNING;
}

/**
 * cpu_yeild function will trigger a tpend exception  and we do the switch in the tpend handler
 * 
*/
extern VOID osTaskSchedule(VOID);  ///<implemented in the ASM

// VOID XPORT_LOS_Schedule(VOID)
// {
//     UINTPTR intSave;
//     intSave = LOS_IntLock();
//     /* Find the highest task */
//     g_losTask.newTask = LOS_DL_LIST_ENTRY(OsPriqueueTop(), LosTaskCB, pendList);
//     /* In case that running is not highest then reschedule */
//     if (g_losTask.runTask != g_losTask.newTask) {
//         if (!g_losTaskLock) {
//             LOS_IntRestore(intSave);
//             osTaskSchedule();
//             return;
//         }
//     }
//     LOS_IntRestore(intSave);
// }


// VOID OsSchedule(VOID)
// {
//     asmOSTaskSchedule();
// }

UINT32 *Los_HWStackInit(VOID *topStack, UINT32 stackSize, TaskFunction_t pxCode, void *pvParameters)
{
    UINT32 *stk  = NULL;
    VOID *start, *end;

    /*
     * Reserve 196 bytes at high address of stack to save registers.
     * Other spaces of stack will fill with LiteOS task magic number
     * and OS_TASK_STACK_INIT.
     */
    start = topStack;
    end = topStack + stackSize;

    #define REG_RESERVED_SIZE 196
    if (end - start > REG_RESERVED_SIZE) {
        end -= REG_RESERVED_SIZE;
        stk = (UINT32 *)topStack;
        *stk = OS_TASK_MAGIC_WORD;
        stk++;
        while (stk < (UINT32 *)end) {
            *stk++ = (INT32)OS_TASK_STACK_INIT;
        }
    }
    #undef REG_RESERVED_SIZE

    stk = topStack + stackSize;

    *(--stk)  = (UINT32)pxCode;            /* Entry Point                                         */
    *(--stk)  = (UINT32)0x80000340L;       /* PSR                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR15                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR14                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR13                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR12                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR11                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR10                                                */
    *(--stk)  = (UINT32)0x12345678L;       /* VR9                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR8                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR7                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR6                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR5                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR4                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR3                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR2                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR1                                                 */
    *(--stk)  = (UINT32)0x12345678L;       /* VR0                                                 */
    *(--stk)  = (UINT32)0x31313131L;       /* R31                                                 */
    *(--stk)  = (UINT32)0x30303030L;       /* R30                                                 */
    *(--stk)  = (UINT32)0x29292929L;       /* R29                                                 */
    *(--stk)  = (UINT32)0x28282828L;       /* R28                                                 */
    *(--stk)  = (UINT32)0x27272727L;       /* R27                                                 */
    *(--stk)  = (UINT32)0x26262626L;       /* R26                                                 */
    *(--stk)  = (UINT32)0x25252525L;       /* R25                                                 */
    *(--stk)  = (UINT32)0x24242424L;       /* R24                                                 */
    *(--stk)  = (UINT32)0x23232323L;       /* R23                                                 */
    *(--stk)  = (UINT32)0x22222222L;       /* R22                                                 */
    *(--stk)  = (UINT32)0x21212121L;       /* R21                                                 */
    *(--stk)  = (UINT32)0x20202020L;       /* R20                                                 */
    *(--stk)  = (UINT32)0x19191919L;       /* R19                                                 */
    *(--stk)  = (UINT32)0x18181818L;       /* R18                                                 */
    *(--stk)  = (UINT32)0x17171717L;       /* R17                                                 */
    *(--stk)  = (UINT32)0x16161616L;       /* R16                                                 */
    *(--stk)  = (UINT32)0xfffffffeL;       /* R15 (LR) (init value will cause fault if ever used) */
    *(--stk)  = (UINT32)0x13131313L;       /* R13                                                 */
    *(--stk)  = (UINT32)0x12121212L;       /* R12                                                 */
    *(--stk)  = (UINT32)0x11111111L;       /* R11                                                 */
    *(--stk)  = (UINT32)0x10101010L;       /* R10                                                 */
    *(--stk)  = (UINT32)0x09090909L;       /* R9                                                  */
    *(--stk)  = (UINT32)0x08080808L;       /* R8                                                  */
    *(--stk)  = (UINT32)0x07070707L;       /* R7                                                  */
    *(--stk)  = (UINT32)0x06060606L;       /* R6                                                  */
    *(--stk)  = (UINT32)0x05050505L;       /* R5                                                  */
    *(--stk)  = (UINT32)0x04040404L;       /* R4                                                  */
    *(--stk)  = (UINT32)0x03030303L;       /* R3                                                  */
    *(--stk)  = (UINT32)0x02020202L;       /* R2                                                  */
    *(--stk)  = (UINT32)0x01010101L;       /* R1                                                  */
    *(--stk)  = (UINT32)pvParameters;      /* R0 : Argument                                       */

    return stk;
}

extern void OsTaskEntry(unsigned int taskID);
VOID *XPORT_OsTskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack)
{
	UINT32 *ret;
	ret = Los_HWStackInit(topStack, stackSize, (TaskFunction_t)OsTaskEntry, (void *)taskID);
	return ret;
}

extern long PortSaveLocalPSR();
extern void PortRestoreLocalPSR();
extern void PortEnableInterrupt();

extern BOOL g_taskScheduled;
extern void LOS_StartToRunFirst();

void  XPORT_LOS_StartToRun()
{
	g_taskScheduled = TRUE;
	LOS_TaskTCBUpdate();
	LOS_StartToRunFirst();
	return;
}

UINT32 XPORT_LOS_IntLock(void)
{
	long oldInterruptLevel = PortSaveLocalPSR();
    return (UINT32)oldInterruptLevel;
}

#if 0
extern long GetCurrentPSR();
UINT32 LOS_IntUnLock(void)
{
    long ret = GetCurrentPSR(); 
    PortEnableInterrupt();
    return (UINT32)ret;   
}
#endif

VOID XPORT_LOS_IntRestore(UINT32 intSave)
{
	PortRestoreLocalPSR((long)intSave);
    return;
}

size_t strnlen(const char *s, size_t n)
{
	const char *p = memchr(s, 0, n);
	return p ? p-s : n;
}

size_t wcrtomb(char *restrict s, int wc, void *restrict st)
{
	return -1;
}

int wctomb(char *s, int wc)
{
	if (!s) return 0;
	return wcrtomb(s, wc, 0);
}

