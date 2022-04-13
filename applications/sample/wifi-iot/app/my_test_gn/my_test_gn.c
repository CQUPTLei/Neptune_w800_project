#include<stdio.h>
#include "ohos_init.h"

void my_test_gn(void)
{
    int i=5;
    while(i>0)
    {
        printf("Hello,my_test\n");
        i-=1;
    }
    
}

SYS_RUN(my_test_gn);