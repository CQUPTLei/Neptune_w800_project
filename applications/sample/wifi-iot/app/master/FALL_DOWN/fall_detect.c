#include "fall_detect.h"

/***************************************************************
  * 函数功能: 计算加速度幅值和accsmv
  * 输入参数: 三轴加速度
  * 返 回 值: 三轴合加速度
  * 说    明: 无
 ***************************************************************/ 
float acc_smv_cul(float ax,float ay, float az)
{
    return abs(ax)+fabs(ay)+fabs(az);
}


/***************************************************************
  * 函数功能: 计算三轴合角速度
  * 输入参数: 三轴角速度
  * 返 回 值: 三轴合角速度
  * 说    明: 无
 ***************************************************************/ 
float w_gyr_cul(float gx, float gy, float gz)
{
    return sqrt( pow(gx,2) + pow(gy,2) + pow(gz,2) );
}

/***************************************************************
  * 函数功能: 计算姿态角
  * 输入参数: x、y轴加速度
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
float pose_cul(float r,float p)
{
    return fabs(r) + fabs(p);
}

/***************************************************************
  * 函数功能: 跌倒检测---可疑
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 人摔倒时，加速度变化较为剧烈，且响应速度较快，所以最先判断加速度变化。
  *          若加速度幅值和超过阈值，则判断合角速度ωgyr是否超过阈值。若合角速度超过阈值，则进行下一步判断。
 ***************************************************************/ 
int suspect_fall_detect(float gx, float gy, float gz, float ax, float ay, float az,float r,float y)
{
    if (acc_smv_cul(ax,ay,az)> acc_smv_threshold)
    {
        if(w_gyr_cul(gx,gy,gz) > w_gyr_threshold)
        {
            if(pose_cul(r,y) < angle_threshold)
            {
                printf("FALL DOWN !!!\n");
                return 1;
            }
            else return 0;
        }
        else return 0;
    }
    else return 0;
    
}