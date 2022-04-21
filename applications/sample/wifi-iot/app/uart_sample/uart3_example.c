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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_uart.h"
#include "wifiiot_uart_ex.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"


#define UART_TASK_STACK_SIZE 512
#define UART_TASK_PRIO 25
#define TEST_UART_SIZE 8
#define INIT_BAUD_RATE 115200
#define TEST_BUF_SIZE 2048
#define UART_PORT WIFI_IOT_UART_IDX_3
unsigned char uart3ReadBuff[2048] = {0};

static void *UartTask(const char *arg)
{
    (void)arg;
    unsigned int ret;
    int len;
    unsigned char uartWriteBuff[] = "hello uart!";

    WifiIotUartAttribute uartAttr;
    uartAttr.baudRate = INIT_BAUD_RATE,
    uartAttr.dataBits = WIFI_IOT_UART_DATA_BIT_8;
    uartAttr.stopBits = WIFI_IOT_UART_STOP_BIT_1;
    uartAttr.parity = WIFI_IOT_UART_PARITY_NONE;
    uartAttr.pad ='M';

    WifiIotUartExtraAttr extraAttr;
    (void)memset_s(&extraAttr, sizeof(WifiIotUartExtraAttr), 0, sizeof(WifiIotUartExtraAttr));
    extraAttr.txFifoLine = WIFI_IOT_FIFO_LINE_ONE_EIGHT;
    extraAttr.rxFifoLine = WIFI_IOT_FIFO_LINE_SEVEN_EIGHTS;
    extraAttr.flowFifoLine = WIFI_IOT_FIFO_LINE_ONE_EIGHT;
    extraAttr.txBlock = WIFI_IOT_UART_BLOCK_STATE_BLOCK;
    extraAttr.rxBlock = WIFI_IOT_UART_BLOCK_STATE_BLOCK;
    extraAttr.txBufSize = TEST_BUF_SIZE;
    extraAttr.rxBufSize = TEST_BUF_SIZE;
    extraAttr.txUseDma = WIFI_IOT_UART_USE_DMA;
    extraAttr.rxUseDma = WIFI_IOT_UART_USE_DMA;

    ret = UartDeinit(UART_PORT);

    ret = UartInit(UART_PORT, &uartAttr, &extraAttr);

    len = UartWrite(UART_PORT, uartWriteBuff, sizeof(uartWriteBuff));

    while (1) {
        len = UartRead(UART_PORT, uart3ReadBuff, sizeof(uart3ReadBuff));
        if(len > 0)
            UartWrite(UART_PORT, uart3ReadBuff, len);
    }

    return NULL;
}

static void UartExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "UartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = UART_TASK_STACK_SIZE;
    attr.priority = UART_TASK_PRIO;
    printf("[UartExample] create UartTask!\n");
    if (osThreadNew((osThreadFunc_t)UartTask, NULL, &attr) == NULL) {
        printf("[UartExample] Falied to create UartTask!\n");
    }
}

SYS_RUN(UartExampleEntry);