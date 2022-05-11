#include "max30102.h"
#include <unistd.h>
#include "iot_i2c.h"
#include "iot_errno.h"
#include "stdio.h"

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef   long      int64_t; 
typedef   unsigned   long       uint64_t; 

//延时
void max30102_delay(void)
{
    uint32_t tick;
    const uint32_t timeout = 20U * osKernelGetSysTimerFreq() / 1000000u;

    tick = osKernelGetSysTimerCount(); // get start value of the Kernel system tick
    do {
        ;
    } while(osKernelGetSysTimerCount() - tick < timeout);
}

//写一个字节
void I2C_max30102_write( uint8_t Reg, uint8_t Value)
{
    uint8_t send_data[2] = { Reg,Value };
    printf("\n=========%x============\n",IoTI2cWrite(0,(I2C_WRITE_ADDR<<1)|0x00, send_data,sizeof(send_data))); 
	osDelay(100);
}


// void max30102_i2c_write(uint8_t reg_adder,uint8_t data)
// {
// 	uint8_t transmit_data[2];
// 	transmit_data[0] = reg_adder;
// 	transmit_data[1] = data;
// 	i2c_transmit(transmit_data,2);
// }





//读数据
uint8_t I2C_max30102_read(uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
  
    uint32_t status = 0;
    uint8_t  buffer[1] = {Reg};
    IoTI2cWrite(0,(I2C_READ_ADDR<<1)|0x00, buffer,sizeof(buffer));
    max30102_delay();
    IoTI2cRead(0,(I2C_READ_ADDR<<1)|0x01, pBuffer,Length);
    max30102_delay();
    
    return 0;  
}


//max30102初始化
void max30102_init(void)
{ 
	uint8_t data;
	I2C_max30102_write(MODE_CONFIGURATION,0x40);  //重置
	
	osDelay(100);
	
	I2C_max30102_write(INTERRUPT_ENABLE1,0xE0);
	I2C_max30102_write(INTERRUPT_ENABLE2,0x00);   //中断使能: FIFO almost full flag, new FIFO Data Ready,
																						 	
	I2C_max30102_write(FIFO_WR_POINTER,0x00);     // ambient light cancellation overflow, power ready flag, 
	I2C_max30102_write(FIFO_OV_COUNTER,0x00);
																							//internal temperature ready flag
	I2C_max30102_write(FIFO_RD_POINTER,0x00);    //clear the pointer
	
	I2C_max30102_write(FIFO_CONFIGURATION,0x4F);  //FIFO configuration: sample averaging(1),FIFO rolls on full(0), FIFO almost full value(15 empty data samples when interrupt is issued)  
	
	I2C_max30102_write(MODE_CONFIGURATION,0x03);  //MODE configuration:SpO2 mode
	
	I2C_max30102_write(SPO2_CONFIGURATION,0x2A);  //SpO2 configuration:ACD resolution:15.63pA,sample rate control:200Hz, LED pulse width:215 us 
	
	I2C_max30102_write(LED1_PULSE_AMPLITUDE,0x2f);	//IR LED
	I2C_max30102_write(LED2_PULSE_AMPLITUDE,0x2f);   //RED LED current
	
	I2C_max30102_write(TEMPERATURE_CONFIG,0x01);     //temp
	
	I2C_max30102_read(INTERRUPT_STATUS1,&data,1);
	I2C_max30102_read(INTERRUPT_STATUS2,&data,1);  //clear status
	printf("\n====================Init max30102 OK=======================\n");
}


//读max30102FIFO寄存器
void max30102_fifo_read(float *output_data)
{
    uint8_t receive_data[6];
	uint32_t data[2];
	I2C_max30102_read(FIFO_DATA,receive_data,6);
    data[0] = ((receive_data[0]<<16 | receive_data[1]<<8 | receive_data[2]) & 0x03ffff);
    data[1] = ((receive_data[3]<<16 | receive_data[4]<<8 | receive_data[5]) & 0x03ffff);
	*output_data = data[0];
	*(output_data+1) = data[1];
}

//获取心率
int max30102_getHeartRate(float *input_data,int cache_nums)
{
		float input_data_sum_aver = 0;
		int i,temp;
		
		for(i=0;i<cache_nums;i++)
		{
		input_data_sum_aver += *(input_data+i);
		}
		input_data_sum_aver = input_data_sum_aver/cache_nums;
		for(i=0;i<cache_nums;i++)
		{
				if((*(input_data+i)>input_data_sum_aver)&&(*(input_data+i+1)<input_data_sum_aver))
				{
					temp = i;
					break;
				}
		}
		i++;
		for(;i<cache_nums;i++)
		{
				if((*(input_data+i)>input_data_sum_aver)&&(*(input_data+i+1)<input_data_sum_aver))
				{
					temp = i - temp;
					break;
				}
		}
		if((temp>14)&&(temp<100))
		{
			return 3000/temp;
		}
		else
		{
			return 0;
		}
}


//获取血氧
float max30102_getSpO2(float *ir_input_data,float *red_input_data,int cache_nums)
{
			float ir_max=*ir_input_data,ir_min=*ir_input_data;
			float red_max=*red_input_data,red_min=*red_input_data;
			float R;
			int i;
			for(i=1;i<cache_nums;i++)
			{
				if(ir_max<*(ir_input_data+i))
				{
					ir_max=*(ir_input_data+i);
				}
				if(ir_min>*(ir_input_data+i))
				{
					ir_min=*(ir_input_data+i);
				}
				if(red_max<*(red_input_data+i))
				{
					red_max=*(red_input_data+i);
				}
				if(red_min>*(red_input_data+i))
				{
					red_min=*(red_input_data+i);
				}
			}
			
			 R=((ir_max+ir_min)*(red_max-red_min))/((red_max+red_min)*(ir_max-ir_min));
			return ((-45.060)*R*R + 30.354*R + 94.845);
}