
#ifndef FALL_DETECT_H
#define FALL_DETECT_H

#include  <math.h>
#include <stdio.h>

#define acc_smv_threshold 3        //阈值设置
#define w_gyr_threshold   3.4
#define angle_threshold   50

float acc_smv_cul(float ax,float ay, float az);
float w_gyr_cul(float gx, float gy, float gz);
float pose_cul(float r,float p);
int suspect_fall_detect(float gx, float gy, float gz, float ax, float ay, float az,float r,float y);

#endif
