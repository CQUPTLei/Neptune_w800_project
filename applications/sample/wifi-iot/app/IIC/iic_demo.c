#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "iot_i2c.h"
#include "iot_gpio_w800.h"        //定义了w800引脚，例：IOT_GPIO_PA_00 为PA0
#include "iot_gpio_neptune.h"     //提供了操作设备的API：flash, GPIO, I2C, PWM, UART, and watchdog APIs.可进入查看函数简介、参数、返回等信息


/*
 * iot_i2c.h文件中定义的函数如下
 * unsigned int IoTI2cInit(unsigned int id, unsigned int baudrate);   //初始化IIC设备，设定波特率
 * unsigned int IoTI2cDeinit(unsigned int id);                        //缺省初始化
 * unsigned int IoTI2cWrite(unsigned int id, unsigned short deviceAddr, const unsigned char *data, unsigned int dataLen); //Writes data to an I2C device.
 * unsigned int IoTI2cRead(unsigned int id, unsigned short deviceAddr, unsigned char *data, unsigned int dataLen);        //Reads data from an I2C device.
 * unsigned int IoTI2cSetBaudrate(unsigned int id, unsigned int baudrate);    //Sets the baud rate for an I2C device. 
 */    
 

 // 定义MPU6050内部地址
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取

// #define    IOT_GPIO_PA_01	SCL	//IIC时钟引脚定义
// #define    IOT_GPIO_PA_04	SDA 	//IIC数据引脚定义
unsigned int SCL=IOT_GPIO_PA_01;
unsigned int SDA=IOT_GPIO_PA_04;

// SCL=IOT_GPIO_PA_01;
// SDA=IOT_GPIO_PA_04;

//**************************************
//I2C起始信号
//**************************************
void I2C_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    osDelay(5);                 //延时
    SDA = 0;                    //产生下降沿
    osDelay(5);                 //延时
    SCL = 0;                    //拉低时钟线
}

//**************************************
//I2C停止信号
//**************************************
void I2C_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    osDelay(5);                 //延时
    SDA = 1;                    //产生上升沿
    osDelay(5);                 //延时
}
//**************************************
//I2C发送应答信号
//入口参数:ack (0:ACK 1:NAK)
//**************************************
void I2C_SendACK(int ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    osDelay(5);                 //延时
    SCL = 0;                    //拉低时钟线
    osDelay(5);                 //延时
}
//**************************************
//I2C接收应答信号
//**************************************
int I2C_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    osDelay(5);                 //延时
    int CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    osDelay(5);                 //延时
    return CY;
}
//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(char dat)
{
    char i;
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        osDelay(5);             //延时
        SCL = 0;                //拉低时钟线
        osDelay(5);             //延时
    }
    I2C_RecvACK();
}
//**************************************
//从I2C总线接收一个字节数据
//**************************************
char I2C_RecvByte()
{
    char i;
    char dat = 0;
    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        osDelay(5);             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        osDelay(5);             //延时
    }
    return dat;
}
//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(char REG_Address,char REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，
    I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                   //发送停止信号
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
char Single_ReadI2C(char REG_Address)
{
	char REG_data;
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始	
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	REG_data=I2C_RecvByte();       //读出寄存器数据
	I2C_SendACK(1);                //接收应答信号
	I2C_Stop();                    //停止信号
	return REG_data;
}
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}
//**************************************
//合成数据
//**************************************
int GetData(char REG_Address)
{
	char H,L;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //合成数据
}
//**************************************


static void iicdemo(void)
{
    InitMPU6050();
    osDelay(10);
    while (1)
    {
        printf(GetData(ACCEL_XOUT_H),2,0);
        osDelay(500);
    }
    
}



// static void iicdemoEntry(void)     //程序入口
// {
//     osThreadAttr_t attr;

//     IoTGpioInit(SCL);      //初始化引脚
//     IoTGpioInit(SDA);
    
//     IoTGpioSetDir(SCL, IOT_GPIO_DIR_IN);  //设置引脚输出输出方向， output is 0 PB08 to control led
//     IoTGpioSetDir(SDA,IOT_GPIO_DIR_IN);    // input is PB09

//     IoTIoSetPull(IOT_GPIO_PB_09,IOT_GPIO_PULLHIGH);   //PB9上拉输入，上拉输入的好处就是输入的电平不会上下浮动而导致输入信号不稳定，在没有信号输入的情况下可以稳定在高电平。


//     attr.name = "iicTask";
//     attr.attr_bits = 0U;
//     attr.cb_mem = NULL;
//     attr.cb_size = 0U;
//     attr.stack_mem = NULL;
//     attr.stack_size = 512;
//     attr.priority = 25;

//     if (osThreadNew((osThreadFunc_t)GpioTask, NULL, &attr) == NULL) {
//         printf("[iic_demo] Falied to create iicTask!\n");
//     }
// }

SYS_RUN(iicdemo); // 系统将执行该服务（功能）