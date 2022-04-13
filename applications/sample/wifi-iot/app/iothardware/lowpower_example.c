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

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "lowpower.h"
#include "ohos_init.h"

#define LP_TASK_STACK_SIZE 512
#define LP_TASK_PRIO 25

// USE PA0 High level to wakeup
static void* LowpowerTask(const char* arg)
{
    (void)arg;
    static int i = 0;

    printf("enter LowpowerTask\n");

    while (1) {
        if (i == 0) {
            i++;
            LpcSetType(LIGHT_SLEEP);
        } else
            LpcSetType(DEEP_SLEEP);
        osDelay(500);
    }
}

static void LowpowerExampleEntry(void)
{
    osThreadAttr_t attr;

    LpcInit();

    attr.name = "LowpowerTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LP_TASK_STACK_SIZE;
    attr.priority = LP_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)LowpowerTask, NULL, &attr) == NULL) {
        printf("[LowpowerExample] Falied to create LowpowerTask!\n");
    }
}

SYS_RUN(LowpowerExampleEntry); // if test add it
