#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "iot_gpio_neptune.h"
#include "iot_gpio_w800.h"


//检测按键低电平并亮灯两秒
static void *TrafficLightTask(const char* arg)
{
    (void)arg;
    printf("[user_key] user_key_task...\n");

    IoTGpioInit(IOT_GPIO_PB_05);
    IoTGpioInit(IOT_GPIO_PB_10);
    IoTGpioInit(IOT_GPIO_PA_01);
    IoTGpioInit(IOT_GPIO_PB_01);
    IoTGpioInit(IOT_GPIO_PB_08);

    IoTGpioSetDir(IOT_GPIO_PB_05, 0);//key
    IoTIoSetPull(IOT_GPIO_PB_05,IOT_GPIO_PULLLOW);

    IoTGpioSetDir(IOT_GPIO_PB_10,IOT_GPIO_DIR_OUT);//red
    IoTGpioSetOutputVal(IOT_GPIO_PB_10, 0);

    IoTGpioSetDir(IOT_GPIO_PA_01,IOT_GPIO_DIR_OUT);//green
    IoTGpioSetOutputVal(IOT_GPIO_PA_01, 0);

    IoTGpioSetDir(IOT_GPIO_PB_01,IOT_GPIO_DIR_OUT);//yellow
    IoTGpioSetOutputVal(IOT_GPIO_PB_01, 0);

    IoTGpioSetDir(IOT_GPIO_PB_08, IOT_GPIO_DIR_OUT);//led
    IoTGpioSetOutputVal(IOT_GPIO_PB_08, 1);

    IotGpioValue val;

    uint32_t startPressTicks = 0;
    IotGpioValue lastValue;

    IoTGpioGetInputVal(IOT_GPIO_PB_05, &lastValue);

    while (1) {
        uint32_t ticks = osKernelGetTickCount();

        IotGpioValue value;
        IoTGpioGetInputVal(IOT_GPIO_PB_05, &value);
        if (value == IOT_GPIO_VALUE0) { // 低电平
            if (value != lastValue) {      // 边缘
                startPressTicks = ticks;
            }
            uint32_t pressSeconds = (ticks - startPressTicks) / osKernelGetTickFreq();

            if (pressSeconds >= 3) {
                printf("[user_key] pressed 3s!\n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08, 0);
                IoTGpioSetOutputVal(IOT_GPIO_PB_10, 1);
                IoTGpioSetOutputVal(IOT_GPIO_PA_01, 1);
                IoTGpioSetOutputVal(IOT_GPIO_PB_01, 1);
                osDelay(1000);
                IoTGpioSetOutputVal(IOT_GPIO_PB_08, 1);
                IoTGpioSetOutputVal(IOT_GPIO_PB_10, 0);
                IoTGpioSetOutputVal(IOT_GPIO_PA_01, 0);
                IoTGpioSetOutputVal(IOT_GPIO_PB_01, 0);
            }
        }
        lastValue = value;

        osDelay(100);
    }

    return NULL;
}

static void User_Key(void)
{
    osThreadAttr_t attr = {0};

    printf("[TrafficLightTask] TrafficLightTask...\n");

    attr.name = "TrafficLightTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)TrafficLightTask, NULL, &attr) == NULL)
    {
        printf("[user_key] Falied to create TrafficLightTask!\n");
    }
}

SYS_RUN(User_Key);