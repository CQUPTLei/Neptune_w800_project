#ifndef __IMU_H
#define __IMU_H
#include  <math.h>
#include <stdio.h>


void suspect_fall_detect(float gx, float gy, float gz, float ax, float ay, float az,float Roll,float Yaw);

#endif
