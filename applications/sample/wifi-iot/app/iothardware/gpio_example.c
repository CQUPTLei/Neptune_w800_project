#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio_neptune.h"
#include "iot_gpio_w800.h"

#define LED_TASK_STACK_SIZE 512
#define LED_TASK_PRIO 25

enum LedState {
    LED_ON = 0,
    LED_OFF,
    LED_SPARK,
};

enum LedState g_ledState = LED_SPARK;

static void* GpioTask(const char* arg)
{
    (void)arg;
    while (1) {
        switch (g_ledState) {
            case LED_ON:
                printf(" LED_ON! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,0);
                osDelay(500);
                break;
            case LED_OFF:
                printf(" LED_OFF! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,1);
                osDelay(500);
                break;
            case LED_SPARK:
                printf(" LED_SPARK! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,0);
                osDelay(500);
                IoTGpioSetOutputVal(IOT_GPIO_PB_08, 1);
                osDelay(500);
                break;
            default:
                osDelay(500);
                break;
        }
    }

    return NULL;
}

static void GpioIsr(char* arg)
{
    (void)arg;

    enum LedState nextState = LED_SPARK;

    printf(" GpioIsr entry\n");

    IoTGpioSetIsrMask(IOT_GPIO_PB_09, 0);
    switch (g_ledState) {
        case LED_ON:
            nextState = LED_OFF;
            break;
        case LED_OFF:
            nextState = LED_ON;
            break;
        case LED_SPARK:
            nextState = LED_OFF;
            break;
        default:
            break;
    }

    g_ledState = nextState;
}

static void GpioExampleEntry(void)
{
    osThreadAttr_t attr;

    IoTGpioInit(IOT_GPIO_PB_08);
    IoTGpioInit(IOT_GPIO_PB_09);
    
    IoTGpioSetDir(IOT_GPIO_PB_08, IOT_GPIO_DIR_OUT);// output is 0 PB08 control led

    IoTGpioSetDir(IOT_GPIO_PB_09,IOT_GPIO_DIR_IN);// input is PB09
    IoTIoSetPull(IOT_GPIO_PB_09,IOT_GPIO_PULLHIGH);
    IoTGpioRegisterIsrFunc(IOT_GPIO_PB_09,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);

    attr.name = "GpioTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LED_TASK_STACK_SIZE;
    attr.priority = LED_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)GpioTask, NULL, &attr) == NULL) {
        printf("[GpioExample] Falied to create GpioTask!\n");
    }
}

SYS_RUN(GpioExampleEntry); // if test add it
