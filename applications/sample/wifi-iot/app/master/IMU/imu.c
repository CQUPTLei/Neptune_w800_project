#include "imu.h"

float  q0=1,q1=0,q2=0,q3=0;          
float  exInt=0,eyInt=0,ezInt=0; 

#define	Kp		0.8f                        
#define	Ki		0.001f                         
#define	halfT	0.004f 

//互补滤波得到欧拉角
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
	float  norm;
	float  vx, vy, vz;
	float  ex, ey, ez;

	norm = sqrt(ax*ax + ay*ay + az*az);	//把加速度计的三维向量转成单维向量   
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

		//	下面是把四元数换算成《方向余弦矩阵》中的第三列的三个元素。 
		//	根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，正好是这三个元素
		//	所以这里的vx vy vz，其实就是当前的欧拉角（即四元数）的机体坐标参照系上，换算出来的
		//	重力单位向量。
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3 ;

	ex = (ay*vz - az*vy) ;
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;

	exInt = exInt + ex * Ki;
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	q0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
	q1 = q1 + ( q0*gx + q2*gz - q3*gy) * halfT;
	q2 = q2 + ( q0*gy - q1*gz + q3*gx) * halfT;
	q3 = q3 + ( q0*gz + q1*gy - q2*gx) * halfT;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
 
    //Pitch:俯仰角 精度:0.1°   范围:-90.0° <---> +90.0°
    //Roll:横滚角  精度:0.1°   范围:-180.0°<---> +180.0°
    //Yaw:航向角   精度:0.1°   范围:-180.0°<---> +180.0°
	//Roll   = asin(2*(q0*q1 + q2*q3 )) * 57.2957795f; 

	// Pitch  = asin(2*(q0*q2 - q1*q3 )) * 57.2957795f;                       // 俯仰   换算成度
    // Roll = atan2(2*(q2*q3 + q0*q1), -2*q1*q1-2*q2*q2+1) * 57.2957795f;     // 横滚
    // Yaw  = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.2957795f; // 航向

	Pitch=asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; //俯仰角
	Roll=atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; //横滚角
	Yaw=atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3; //航向角
}

