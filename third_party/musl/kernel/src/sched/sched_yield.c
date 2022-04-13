#include <sched.h>
#include "los_process.h"

int sched_yield()
{
    (void)LOS_TaskYield();
    return 0;
}
