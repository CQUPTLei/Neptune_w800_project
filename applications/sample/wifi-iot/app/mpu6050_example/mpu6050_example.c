/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_pwm.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "imu.h"

MPU6050_Data_TypeDef MPU6050_Data;

//MPU6050获取参数及处理参数
int Gyro_y=0,Gyro_x=0,Gyro_z=0;           //Y轴陀螺仪数据暂存
int Accel_x=0,Accel_y=0,Accel_z=0;	      //X轴加速度值暂存

float  Angle_ax=0.0,Angle_ay=0.0,Angle_az=0.0;  //由加速度计算的加速度(弧度制)
float  Angle_gy=0.0,Angle_gx=0.0,Angle_gz=0.0;  //由角速度计算的角速率(角度制)

//陀螺仪 加计修正参数——————————————————————————————————————
int   g_x=0,g_y=0,g_z=0;            							 //陀螺仪矫正参数
float a_x=0.0,a_y=0.0;  

//四元数解算出的欧拉角 —————————————————————————————————————
float Pitch=0.0,Roll=0.0,Yaw=0.0;                     	//四元数解算出的欧拉角 

void Servo_Angle_Control(uint32_t angle)
{
  float time=0;
  float dutytime=0;
  uint16_t duty=0;
  time=angle/9;
  dutytime=(time+5)/200;
  duty=256*dutytime;
  IoTPwmStart(0,duty,50);
}

static void Example_Task(void)
{
    char buff[20];
    IoTPwmInit(0);           //PWM初始化
    MPU6050_GPIO_Init();     //MPU6050初始化
    ssd1306_Init();
    Servo_Angle_Control(90); //把舵机的初始角度定为90度
    ssd1306_Fill(Black);
    while (1)
    {
        osDelay(5);                     //延时10ms

        MPU6050_Read_Data();            //获取数据
        Accel_y= MPU6050_Data.Accel[0];	//读取6050加速度数据
	    Accel_x= MPU6050_Data.Accel[1] ;		   
	    Accel_z= MPU6050_Data.Accel[2] ;

        Gyro_x = MPU6050_Data.Gyro[0]-g_x;//读取6050角速度数据
	    Gyro_y = MPU6050_Data.Gyro[0]-g_y;
	    Gyro_z = MPU6050_Data.Gyro[0]-g_z;	

        Angle_ax = Accel_x/8192.0;        //根据设置换算
	    Angle_ay = Accel_y/8192.0;
	    Angle_az = Accel_z/8192.0;
	
	    Angle_gx = Gyro_x/65.5*0.0174533;//根据设置换算
	    Angle_gy = Gyro_y/65.5*0.0174533;
	    Angle_gz = Gyro_z/65.5*0.0174533;

        IMUupdate(Angle_gx,Angle_gy,Angle_gz,Angle_ax,Angle_ay,Angle_az);	
        Servo_Angle_Control(Roll+90); //不断的把传感器得到的角度值传入此函数来控制舵机的角度
        ssd1306_SetCursor(2,12);
        sprintf(buff,"横滚角:%0.2f",Roll);
        ssd1306_DrawChinese(2,12,buff, White);
        ssd1306_SetCursor(2,30);
        sprintf(buff,"角度:%0.2f",Roll+90);
        ssd1306_DrawChinese(2,30,buff, White);
        ssd1306_UpdateScreen();
	    printf("Roll:");			  //输出横滚角
	    printf("%f",Roll);
	    printf("\r\n");
    }
}

static void ExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "Example_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 8;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t)Example_Task, NULL, &attr) == NULL)
    {
        printf("Falied to create Example_Task!\n");
    }
}
APP_FEATURE_INIT(ExampleEntry);