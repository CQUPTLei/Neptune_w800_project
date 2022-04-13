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

/**
 * @defgroup los_hw hardware
 * @ingroup kernel
 */

#ifndef _LOS_HW_H
#define _LOS_HW_H

#include "los_base.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */




/* *
 * @ingroup los_hw
 * Check task schedule.
 */
///in the task.c :LOS_TaskCreate call this macro with the g_losTaskLock in the same logic, This  must be a joke: 
///< some one should pay for it, it is not a clean code---TODO
#define LOS_CHECK_SCHEDULE ((!g_losTaskLock))   

/* *
 * @ingroup los_hw
 * Define the type of a task context control block.
 */
///< The context has been defined in the xtensa/xtensa_context.h，we need not analyse it
///< the context save and restore is done by the xtensa interrupt enter and exit, never mind it!
///< here we use the context from the xtensa/xtensa_context.h

/* *
 * @ingroup  los_hw
 * @brief: Task stack initialization.
 *
 * @par Description:
 * This API is used to initialize the task stack.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  taskID     [IN] Type#UINT32: TaskID.
 * @param  stackSize  [IN] Type#UINT32: Total size of the stack.
 * @param  topStack   [IN] Type#VOID *: Top of task's stack.
 *
 * @retval: context Type#TaskContext *.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID *OsTskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack);

/* *
 * @ingroup  los_hw
 * @brief: Task scheduling Function.
 *
 * @par Description:
 * This API is used to scheduling task.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID OsSchedule(VOID);

/* *
 * @ingroup  los_hw
 * @brief: Function to determine whether task scheduling is required.
 *
 * @par Description:
 * This API is used to Judge and entry task scheduling.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID LOS_Schedule(VOID);

/**
 * @ingroup  los_hw
 * @brief: Function to task exit.
 *
 * @par Description:
 * This API is used to exit task.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
LITE_OS_SEC_TEXT_MINOR VOID OsTaskExit(VOID);

/* *
 * @ingroup  los_hw
 * @brief: The M3 wait interrupt instruction.
 *
 * @par Description:
 * This API is used to make CPU enter to power-save mode.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_hw.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID OsEnterSleep(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_HW_H */

