/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: target config
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
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
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include <los_typedef.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


#define _T(x)                                               x

/*=============================================================================
                                        Lib configurable feature configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration lib configurable feature to open
 */
#define LOSCFG_LIB_CONFIGURABLE                             NO

/*=============================================================================
                                       LIBC module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration to support libc
 */
#define LOSCFG_LIB_LIBC                                     NO

/*=============================================================================
                                        System clock module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * System clock (unit: HZ)
 */
#define OS_SYS_CLOCK                                         240000000

/**
 * @ingroup los_config
 * Number of Ticks in one second
 */
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (500UL)


/**
 * @ingroup los_config
 * External configuration item for timer tailoring
 */
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       NO

/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration item for hardware interrupt tailoring
 */
#define LOSCFG_PLATFORM_HWI                                 YES


/*=============================================================================
                                       Task module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Minimum stack size.
 *
 * 0x80 bytes, aligned on a boundary of 4.
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_taskMinStkSize;
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 g_taskMinStkSize
#else
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (ALIGN(0x180, 4))
#endif

/**
 * @ingroup los_config
 * Idle task stack size.
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_taskIdleStkSize;
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                g_taskIdleStkSize
#else
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (0x400UL)
#endif

/**
 * @ingroup los_config
 * Maximum supported number of tasks except the idle task rather than the number of usable tasks
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_taskLimit;
#define LOSCFG_BASE_CORE_TSK_LIMIT                          g_taskLimit
#else
#define LOSCFG_BASE_CORE_TSK_LIMIT                          32
#endif

/**
 * @ingroup los_config
 * Soft timer task stack size
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_taskSwtmrStkSize;
#define LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE               g_taskSwtmrStkSize
#else
#define LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE               (0x1000UL)
#endif

/**
 * @ingroup los_config
 * Default task stack size
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_taskDfltStkSize;
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             g_taskDfltStkSize
#else
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (0x1000UL)
#endif

/**
 * @ingroup los_config
 * Longest execution time of tasks with the same priorities
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_timeSliceTimeOut;
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  g_timeSliceTimeOut
#else
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  10
#endif

/****************************** PMP module configuration ******************************/
/**
 * @ingroup los_config
 * Configuration item for PMP
 */
#define LOSCFG_BASE_CORE_PMP                            NO

/**
 * @ingroup los_config
 * PMP support number : PMP maximum number of region support
 */
#define LOSCFG_PMP_MAX_SUPPORT                          16             // PMP maximum support number

/**
 * @ingroup los_config
 * PMP support address range : from LOSCFG_PMP_MIN_ADDRESS to LOSCFG_PMP_MAX_ADDRESS
 */
#define LOSCFG_PMP_MIN_ADDRESS                         0x00000000UL     // Minimum protected address
#define LOSCFG_PMP_MAX_ADDRESS                         0x07FFFFFFUL     // Maximum protected address

/**
 * @ingroup los_config
 * The PMP entry size granularity must be the multiple of cache line size.
 */
#define CACHE_LINE_SIZE                                32          // cache line size,
#define LOSCFG_PMP_MAX_SIZE                            0xFFFFFFFF

/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Maximum supported number of semaphores
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_semLimit;
#define LOSCFG_BASE_IPC_SEM_LIMIT                           g_semLimit
#else
#define LOSCFG_BASE_IPC_SEM_LIMIT                           100
#endif

/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Maximum supported number of mutexes
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_muxLimit;
#define LOSCFG_BASE_IPC_MUX_LIMIT                           g_muxLimit
#else
#define LOSCFG_BASE_IPC_MUX_LIMIT                           64
#endif

/*=============================================================================
                                       Queue module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Maximum supported number of queues rather than the number of usable queues
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_queueLimit;
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         g_queueLimit
#else
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         64
#endif

/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Maximum supported number of software timers rather than the number of usable software timers
 */
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                               g_swtmrLimit;
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        g_swtmrLimit
#else
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        80
#endif

/*=============================================================================
                                       Memory module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Starting address of the memory
 */
extern UINT8 g_sysMemAddrStart[];
#define OS_SYS_MEM_ADDR                                     (g_sysMemAddrStart)

/**
 * @ingroup los_config
 * Memory size
 */
#define OS_SYS_MEM_SIZE                                     (0x20000)

#define LOSCFG_MEMORY_BESTFIT                               YES

/**
 * @ingroup los_config
 * Configuration module tailoring of membox
 */
#define LOSCFG_MEMBOX                                       NO

/**
 * @ingroup los_config
 * Configuration module momery water line
 */
#define LOSCFG_MEM_WATERLINE                                YES

/**
 * @ingroup los_config
 * Configuration module tailoring of mem node size checking
 */
#define LOSCFG_BASE_MEM_NODE_SIZE_CHECK                     NO

/**
 * @ingroup los_config
 * Configuration module momery alloc and free check
 */
#define LOSCFG_MEM_ENABLE_ALLOC_CHECK                       NO

/*=============================================================================
                                        Exception module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration item for exception tailoring
 */
#define LOSCFG_PLATFORM_EXC                                 NO

/*=============================================================================
                                       Runstop module configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration item for runstop
 */
#define LOSCFG_RUNSTOP_THRESHOLD                            2

/*=============================================================================
                                        track configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration item for track
 */
#define LOSCFG_BASE_MISC_TRACK                          NO

/**
 * @ingroup los_config
 * Max count of track items
 */
#define LOSCFG_BASE_MISC_TRACK_MAX_COUNT                    1000

/**
 * @ingroup los_config
 * Configuration item for task (stack) monitoring module tailoring
 */
#define LOSCFG_BASE_CORE_TSK_MONITOR                        NO

/*=============================================================================
                                        CPUP configuration
=============================================================================*/
#if (LOSCFG_BASE_CORE_TSK_MONITOR == YES)
/**
 * @ingroup los_config
 * Configuration item for CPU task usage tailoring
 */
#define LOSCFG_BASE_CORE_CPUP                               YES
/**
 * @ingroup los_config
 * Configuration item for CPU hwi usage tailoring
 */
#if (LOSCFG_BASE_CORE_CPUP == YES)
#define LOSCFG_BASE_CORE_CPUP_HWI                           YES
#endif

#endif

/*=============================================================================
                                        FPB configuration
=============================================================================*/
/**
 * @ingroup los_config
 * Configuration item for flash patch
 */
#define LOSCFG_BASE_CORE_FPB                                NO

/**
 * @ingroup los_config
 * flash patch type
 */
#define LOSCFG_FPB_CONFIG_PCO                               1

#ifdef LOSCFG_DEBUG_KASAN
#if (LOSCFG_LIB_CONFIGURABLE == YES)
extern UINT32                                              g_kasanLimit;
#define LOSCFG_KASAN_TASK_STACK_NUM                        g_kasanLimit
#else
#define LOSCFG_KASAN_TASK_STACK_NUM                        3
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
