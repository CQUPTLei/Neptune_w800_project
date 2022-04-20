/*GPIO、中断使用练习
LED2和PB8相连，共阳，则PB8输出0点亮
PB9按键改变LED状态（中断方式）
使用了CMSIS实时操作系统，也可以修改为普通的轮询、中断等结构
*/

#include <stdio.h>               //基本输入输出函数的声明

#include <unistd.h>              //unistd.h为Linux/Unix系统中内置头文件，包含了许多系统服务的函数原型，例如read函数、write函数和getpid函数等。
                                 //其作用相当于windows操作系统的"windows.h"，是操作系统为用户提供的统一API接口，方便调用系统提供的一些服务。

#include "cmsis_os2.h"           //CMSIS-RTOS2 是一个通用的 API ，不受底层 RTOS 内核的影响。应用程序员在用户代码中调用 CMSIS-RTOS2 API
                                 // 函数以确保从一个 RTOS 到另一个 RTOS 的最大可移植性。使用 CMSIS-RTOS2 API 的中间件可以避免不必要的移植工作。
#include "ohos_init.h"           //系统初始化启动服务、功能的头文件，SYS_RUN（）这个宏就定义在其中
#include "iot_gpio_neptune.h"     //提供了操作设备的API：flash, GPIO, I2C, PWM, UART, and watchdog APIs.可进入查看函数简介、参数、返回等信息
#include "iot_gpio_w800.h"        //定义了w800引脚，例：IOT_GPIO_PA_00 为PA0

#define LED_TASK_STACK_SIZE 512  //此处宏用于RTOS的设置。RTOS特点：1)多任务; 2)有线程优先级; 3)多种中断级别。
#define LED_TASK_PRIO 25

enum LedState {       //枚举LED状态，凉、灭、闪烁
    LED_ON = 0,
    LED_OFF,
    LED_SPARK,
};

enum LedState g_ledState = LED_SPARK;     //初始为闪烁

static void* GpioTask(const char* arg)    //创建GPIO任务
{
    (void)arg;
    while (1) {
        switch (g_ledState) {            //扫描led状态
            case LED_ON:
                printf(" LED_ON! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,0);
                osDelay(1000);
                break;
            case LED_OFF:
                printf(" LED_OFF! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,1);
                osDelay(1000);
                break;
            case LED_SPARK:
                printf(" LED_SPARK! \n");
                IoTGpioSetOutputVal(IOT_GPIO_PB_08,0);    //闪烁，osDelay单位ms,他是CMSIS实时操作系统中的函数
                osDelay(200);
                IoTGpioSetOutputVal(IOT_GPIO_PB_08, 1);
                osDelay(200);
                break;
            default:
                osDelay(500);
                break;
        }
    }

    return NULL;
}

static void GpioIsr(char* arg)             //GPIO中断函数，PB9中断输入，进入一次中断状态改变一次
{
    (void)arg;

    enum LedState nextState = LED_SPARK;

    printf(" GpioIsr entry\n");

    IoTGpioSetIsrMask(IOT_GPIO_PB_09, 0);  //不屏蔽PB9的中断功能（因为用它的中断功能确定LED的状态）
    switch (g_ledState) {                   //初始状态为SPARK
        case LED_ON:
            nextState = LED_SPARK;
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

static void GpioExampleEntry(void)     //程序入口
{
    osThreadAttr_t attr;

    IoTGpioInit(IOT_GPIO_PB_08);      //初始化PB8、9引脚
    IoTGpioInit(IOT_GPIO_PB_09);
    
    IoTGpioSetDir(IOT_GPIO_PB_08, IOT_GPIO_DIR_OUT);  //设置引脚输出输出方向， output is 0 PB08 to control led
    IoTGpioSetDir(IOT_GPIO_PB_09,IOT_GPIO_DIR_IN);    // input is PB09

    IoTIoSetPull(IOT_GPIO_PB_09,IOT_GPIO_PULLHIGH);   //PB9上拉输入，上拉输入的好处就是输入的电平不会上下浮动而导致输入信号不稳定，在没有信号输入的情况下可以稳定在高电平。
    IoTGpioRegisterIsrFunc(IOT_GPIO_PB_09,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);  //中断使能，PB9,边缘触发、下降沿或低电平

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

SYS_RUN(GpioExampleEntry); // 系统将执行该服务（功能）
