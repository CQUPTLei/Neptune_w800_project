#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "iot_i2c.h"
#include "iot_gpio_w800.h"        //定义了w800引脚，例：IOT_GPIO_PA_00 为PA0
#include "iot_gpio_neptune.h"     //提供了操作设备的API：flash, GPIO, I2C, PWM, UART, and watchdog APIs.可进入查看函数简介、参数、返回等信息

#include "wm_gpio.h"
#include "wm_i2c.h"
typedef uint8_t u8;

#define I2C_SCL_PIN		IOT_GPIO_PA_01			/* 连接SCL的GPIO PA1*/
#define I2C_SDA_PIN		IOT_GPIO_PA_04			/* 连接SDL的GPIO PA4*/

#define I2C_SCL_1()  IoTGpioSetOutputVal(I2C_SCL_PIN,1)		/* SCL = 1 */
#define I2C_SCL_0()  IoTGpioSetOutputVal(I2C_SCL_PIN,0)		/* SCL = 0 */

#define I2C_SDA_1()  IoTGpioSetOutputVal(I2C_SDA_PIN,1)		/* SDA = 1 */
#define I2C_SDA_0()  IoTGpioSetOutputVal(I2C_SDA_PIN,0)		/* SDA = 0 */
#define I2C_SDA_READ() tls_gpio_read(I2C_SDA_PIN)
// static int I2C_SDA_READ(void)
// {
//     IotGpioValue *data;   
//     IoTGpioGetInputVal(I2C_SDA_PIN,data);
//     return *data;
// }


//=================MPU6050地址====================
#define MPU6050_RA_SMPLRT_DIV       0x19
#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C

#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_PWR_MGMT_2       0x6C

#define MPU6050_WHO_AM_I        0x75
#define MPU6050_SMPLRT_DIV      0  //8000Hz  fix me
#define MPU6050_DLPF_CFG        0  //fix me
#define MPU6050_GYRO_OUT        0x43     //MPU6050陀螺仪数据寄存器地址
#define MPU6050_ACC_OUT         0x3B     //MPU6050加速度数据寄存器地址
#define MPU6050_RA_TEMP_OUT_H   0x41     //温度
#define	MPU6050_SLAVE_ADDRESS	0xD0	//IIC写入时的地址字节数据，+1为读


//==========================函数声明=====================
static void i2c_Ack(void);
static void i2c_NAck(void);

//==========================驱动========================
static void i2c_Delay(void)
{
	uint8_t i;
	for (i = 0; i < 10; i++);
}

//当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
static void i2c_Start(void)
{
    /*    _____
     *SDA      \_____________
     *    __________
     *SCL           \________
     */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

//当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
static void i2c_Stop(void)
{
    /*               _______
     *SDA __________/
     *          ____________
     *SCL _____/
     */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/*CPU向I2C总线设备发送8bit数据*/
static void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
	/* 先发送高7位，*/
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();//发送的1
		}
		else
		{
			I2C_SDA_0();//发送的0
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();	
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); //释放总线
		}
		_ucByte <<= 1;	/*左移一个 bit */
		i2c_Delay();
	}
}

/*CPU从I2C设备读取8bit数据*/
uint8_t i2c_ReadByte(u8 ack)
{
	uint8_t i;
	uint8_t value;
	/* 读取到第一个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	if(ack==0)
		i2c_NAck();
	else
		i2c_Ack();
	return value;
}

/*CPU产生一个时钟，并读取器件的ACK应答信号*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU驱动SCL = 1, 此器件会返回ACK应答 */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU读取SDA口状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*CPU产生一个ACK信号*/
static void i2c_Ack(void)
{

    /*           ____
     *SCL ______/    \______
     *    ____         _____
     *SDA     \_______/
     */
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*CPU产生1个NACK信号*/
static void i2c_NAck(void)
{
    /*           ____
     *SCL ______/    \______
     *    __________________
     *SDA
     */
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生一个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();	
}

/*配置I2C总线的GPIO*/
void i2c_GPIO_Config(void)
{
    IoTGpioInit(IOT_GPIO_PA_01);      //初始化PB8、9引脚
    IoTGpioInit(IOT_GPIO_PA_04);
    
    IoTGpioSetDir(IOT_GPIO_PA_01, IOT_GPIO_DIR_OUT);  //设置引脚输出输出方向
    IoTGpioSetDir(IOT_GPIO_PA_04,IOT_GPIO_DIR_OUT);    

    IoTIoSetPull(IOT_GPIO_PA_01,IOT_GPIO_PULLHIGH);   
	IoTIoSetPull(IOT_GPIO_PA_04,IOT_GPIO_PULLHIGH); 
    //IoTGpioRegisterIsrFunc(IOT_GPIO_PB_09,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);  //中断使能，PB9,边缘触发、下降沿或低电平
	i2c_Stop();
}

//=======================读写数据=====================================
void MPU6050_WriteReg(u8 reg_add,u8 reg_dat)
{
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	i2c_SendByte(reg_dat);
	i2c_WaitAck();
	i2c_Stop();
}

void MPU6050_ReadData(u8 reg_add,unsigned char*Read,u8 num)
{
	unsigned char i;
	
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS+1);
	i2c_WaitAck();
	
	for(i=0;i<(num-1);i++){
		*Read=i2c_ReadByte(1);
		Read++;
	}
	*Read=i2c_ReadByte(0);
	i2c_Stop();
}

//==================初始化==========================

void MPU6050_Init(void)
{
  int i=0,j=0;
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++);
  }
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x01);
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	//值得设置
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x03);//值得设置
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x00);
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);
// 	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x80);
// 	osDelay(100);
// 	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x00);
// 	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x00);
// 	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x01);
// 	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_2,0x00);
}



// u8 MPU_Init(void)
// {
//     u8 res;
//     IIC_Init();//初始化IIC总线
//     IIC_Write_Byte(MPU_PWR_MGMT1_REG,0X80);//复位MPU6050
//     delay_ms(100);
//     IIC_Write_Byte(MPU_PWR_MGMT1_REG,0X00);//唤醒MPU6050
//     MPU_Set_Gyro_Fsr(3); //陀螺仪传感器,±2000dps
//     MPU_Set_Accel_Fsr(0); //加速度传感器 ±2g
//     MPU_Set_Rate(50); //设置采样率50HZ
//     IIC_Write_Byte(MPU_INT_EN_REG,0X00); //关闭所有中断
//     IIC_Write_Byte(MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
//     IIC_Write_Byte(MPU_FIFO_EN_REG,0X00);//关闭FIFO
//     IIC_Write_Byte(MPU_INTBP_CFG_REG,0X80);//INT引脚低电平有效
//     res=IIC_Read_Byte(MPU_DEVICE_ID_REG);
//     if(res==MPU_ADDR)//器件ID正确
//     {
//         IIC_Write_Byte(MPU_PWR_MGMT1_REG,0X01);//设置CLKSEL,PLL X 轴为参考
//         IIC_Write_Byte(MPU_PWR_MGMT2_REG,0X00);//加速度陀螺仪都工作
//         MPU_Set_Rate(50); //设置采样率为50HZ
//     }else return 1;
//     return 0;

// }



/*
CPU发送设备地址，然后读取设备应答是否有设备
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_GPIO_Config();		/* 配置GPIO */
	
	i2c_Start();		/*I2C开始信号*/

	/* 发送设备地址和读写控制bit（0=写，1=读）bit7位先传 */
	i2c_SendByte(_Address|I2C_WR);
	ucAck = i2c_WaitAck();	/*检测设备应答 */

	i2c_Stop();			/* I2C结束信号 */

	return ucAck;
}




//=====================传感器数据读取======================
void MPU6050ReadGyro(short *gyroData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}


void MPU6050ReadAcc(short *accData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}



void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	u8 buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2); 
    temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;
}

static void iic_main(void)
{
	short Accel[3];
	short Gyro[3];
	float Temp;
	i2c_GPIO_Config();
	tls_i2c_init(400000);
    i2c_Stop();
	MPU6050_Init();
	while(1)
	{
		MPU6050ReadAcc(Accel);			
		printf("\n加速度 %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
		MPU6050ReadGyro(Gyro);
		printf("陀螺仪 %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);				
		MPU6050_ReturnTemp(&Temp); 
		printf("温度 %8.2f",Temp);				
		osDelay(10);
	}   
}

SYS_RUN(iic_main);