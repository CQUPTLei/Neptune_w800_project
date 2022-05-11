#include "stdio.h"
#include "max30102.h"

#include "ohos_init.h"
#include "iot_i2c.h"

#include "cmsis_os2.h"           
#include "iot_gpio_neptune.h"     //提供了操作设备的API：flash, GPIO, I2C, PWM, UART, and watchdog APIs.可进入查看函数简介、参数、返回等信息
#include "iot_gpio_w800.h"   

#define CACHE_NUMS 150                  //缓存数
#define PPG_DATA_THRESHOLD 100000 	    //检测阈值
uint8_t max30102_int_flag=0;  	       	//中断标志

float ppg_data_cache_RED[CACHE_NUMS]={0};   //缓存区
float ppg_data_cache_IR[CACHE_NUMS]={0};    //缓存区


void max30102TASK(void)
{
  //初始化

	//延时
  osDelay(500);

  printf("\n==========%d============\n",IoTI2cInit(0, 400*1000));
	max30102_init();


	//max30102_fir_init();

	uint16_t cache_counter=0;                   //缓存计数器
	float max30102_data[2],fir_output[2];

  while (1)
  {
		if(max30102_int_flag)			//中断信号产生
			{
					max30102_int_flag = 0;
					//max30102_fifo_read(max30102_data);		//读取数据
				
					//ir_max30102_fir(&max30102_data[0],&fir_output[0]);
					//red_max30102_fir(&max30102_data[1],&fir_output[1]);  //滤波

					// if((max30102_data[0]>PPG_DATA_THRESHOLD)&&(max30102_data[1]>PPG_DATA_THRESHOLD))  //大于阈值，说明传感器有接触
					// {		
					// 		ppg_data_cache_IR[cache_counter]=fir_output[0];
					// 		ppg_data_cache_RED[cache_counter]=fir_output[1];
					// 		cache_counter++;
					// }
					// else				//小于阈值
					// {
					// 		cache_counter=0;
					// }


					if(cache_counter>=CACHE_NUMS)  //收集满了数据
					{
						printf("心率：%d  次/min   ",max30102_getHeartRate(ppg_data_cache_IR,CACHE_NUMS));
						printf("血氧：%.2f  %%\n",max30102_getSpO2(ppg_data_cache_IR,ppg_data_cache_RED,CACHE_NUMS));
						cache_counter=0;
					}
			}
  }
 
}


static void GpioIsr(char* arg)             //GPIO中断函数，PB0中断输入，进入一次中断状态改变一次
{
    (void)arg;
    max30102_int_flag=1;
}



static void max30102TASKEnter(void)     //程序入口
{
    osThreadAttr_t attr;

    IoTGpioInit(IOT_GPIO_PB_00);      //初始化PB0引脚


    IoTGpioSetDir(IOT_GPIO_PB_00,IOT_GPIO_DIR_IN);    // input is PB00
    IoTIoSetPull(IOT_GPIO_PB_00,IOT_GPIO_PULLHIGH);   


    IoTGpioRegisterIsrFunc(IOT_GPIO_PB_00,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);  //中断使能，PB0,边缘触发、下降沿或低电平

    attr.name = "max30102TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority =  osPriorityNormal;

    if (IoTI2cInit(0, 400*1000)) {
       printf("I2C Init Failed\n");
    }
    if (osThreadNew((osThreadFunc_t)max30102TASK, NULL, &attr) == NULL) {
      printf("[max30102TASK] Falied to create max30102TASK!\n");
    }
}

SYS_RUN(max30102TASKEnter); // 系统将执行该服务（功能）
