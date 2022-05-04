#include "fall_detect.h"

#define acc_smv_threshold 3        //阈值设置
#define w_gyr_threshold   3.4
#define angle_threshold   50

float acc_smv=0;
double w_gyr=0;
float  angle=0;
/***************************************************************
  * 函数功能: 计算加速度幅值与accsmv
  * 输入参数: 三轴加速度
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void acc_smv_cul(float ax,float ay, float az)
{
    acc_smv=fabs(ax)+fabs(ay)+fabs(az);
}


/***************************************************************
  * 函数功能: 计算三轴合角速度
  * 输入参数: 三轴角速度
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void w_gyr_cul(float gx, float gy, float gz)
{
    w_gyr=sqrt( pow(gx,2) + pow(gy,2) + pow(gz,2) );
}

/***************************************************************
  * 函数功能: 计算姿态角
  * 输入参数: x、y轴加速度
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void pose_cul(float Roll,float Yaw)
{
    // float angle_x=0;
    // float angle_y=0;
    // angle_x=-atan2(ax,ay);
    // angle_y=asin(angle_x/(sqrt(pow(ax,2)+pow(ay,2))));
    // angle=fabs(angle_x) + fabs(angle_y);
    angle=fabs(Roll) + fabs(Yaw);

}

/***************************************************************
  * 函数功能: 跌倒检测---可疑
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 人摔倒时，加速度变化较为剧烈，且响应速度较快，所以最先判断加速度变化。
  *          若加速度幅值和超过阈值，则判断合角速度ωgyr是否超过阈值。若合角速度超过阈值，则进行下一步判断。
 ***************************************************************/ 
void suspect_fall_detect(float gx, float gy, float gz, float ax, float ay, float az,float Roll,float Yaw)
{
    acc_smv_cul(ax,ay,az);
    w_gyr_cul(gx,gy,gz);
    pose_cul(Roll,Yaw);

    if (acc_smv > acc_smv_threshold)
    {
        if(w_gyr > w_gyr_threshold)
        {
            if(angle < angle_threshold)
            {
                printf("FALL DOWN !!!\n");
            }
        }
    }
    
}