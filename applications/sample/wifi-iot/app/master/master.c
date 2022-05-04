#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_pwm.h"
#include "mpu6050.h"
#include "imu.h"
#include "fall_detect.h"

MPU6050_Data_TypeDef MPU6050_Data;

//MPU6050获取参数及处理参数
int Gyro_y=0,Gyro_x=0,Gyro_z=0;           //Y轴陀螺仪数据暂存
int Accel_x=0,Accel_y=0,Accel_z=0;	      //X轴加速度值暂存

float  Angle_ax=0.0,Angle_ay=0.0,Angle_az=0.0;  //由加速度计算的加速度(弧度制)
float  Angle_gy=0.0,Angle_gx=0.0,Angle_gz=0.0;  //由角速度计算的角速率(角度制)

int   g_x=0,g_y=0,g_z=0;            					//陀螺仪矫正参数
float a_x=0.0,a_y=0.0;  

float Pitch=0.0,Roll=0.0,Yaw=0.0;                     	//四元数解算出的欧拉角 



static void Example_Task(void)
{
    char buff[20];
    MPU6050_GPIO_Init();                 //MPU6050初始化
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

	    // printf("Roll:%.2f",Roll);			  //输出横滚角,绕z轴 
        // printf("    Yaw:%.2f",Yaw);			  //y
        // printf("    Pitch:%.2f",Pitch);	  //x
        // printf("    Accel_x:%.2f",Angle_ax);
        // printf("    Accel_y:%.2f",Angle_ay);
        // printf("    Accel_z:%.2f",Angle_az);
        // printf("    Angle_x:%.2f",Angle_gx);
        // printf("    Angle_y:%.2f",Angle_gy);
        // printf("    Angle_z:%.2f\n",Angle_gz);
        suspect_fall_detect(Angle_gx,Angle_gy,Angle_gz,Angle_ax,Angle_ay,Angle_az,Roll,Yaw);
        
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