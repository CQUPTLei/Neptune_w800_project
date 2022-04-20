#include <string.h>
#include "wm_include.h"
#include "wm_config.h"
#include "iperf.h"
#include "wm_cpu.h"
#include "iperf_error.h"

#if TLS_CONFIG_WIFI_PERF_TEST
extern int tls_perf(void* data);

#define THT_QUEUE_SIZE	32
#define THT_TASK_PRIO	32
#define THT_TASK_STACK_SIZE 1024
struct tht_param gThtSys;
tls_os_queue_t *tht_q = NULL;
#if TLS_OS_FREERTOS
u32 *ThtTaskStk = NULL;
#endif
int testing = 0;
u8  iperf_cpu_freq = 0;
int exit_last_test = 0;
void tht_task(void *sdata)
{
	void *tht = (struct tht_param *)sdata;
	void *msg;
	for(;;) 
	{
		printf("\n tht_task \n");
		//msg = OSQPend(tht_q, 0, &error);
	    tls_os_queue_receive(tht_q,(void **)&msg,0,0);

		printf("\n msg =%d\n",(u32)msg);
		switch((u32)msg)
		{
			case TLS_MSG_WIFI_PERF_TEST_START:
				tls_sys_clk_set(CPU_CLK_240M);
				printf("\nTHT_TEST_START\n");
				tls_perf(tht);
				break;
			default:
				break;
		}
	}

}


void CreateThroughputTask(void)
{
    if(!testing)
    {
#if TLS_OS_LITEOS
		memset(&gThtSys, 0 ,sizeof(struct tht_param));
		iperf_cpu_freq = tls_reg_read32(HR_CLK_DIV_CTL)&0xFF;

		tls_os_queue_create(&tht_q, THT_QUEUE_SIZE);

		tls_os_task_create(NULL, NULL,
						   tht_task,
						   (void *)&gThtSys,
						   (void *)NULL,
						   THT_TASK_STACK_SIZE * sizeof(u32),
						   THT_TASK_PRIO,
						   0);

		testing = 1;
		printf("CreateThroughputTask\n");
#else
        ThtTaskStk = (u32 *)tls_mem_alloc(THT_TASK_STACK_SIZE * sizeof(u32));
        if (ThtTaskStk)
        {
            memset(&gThtSys, 0 ,sizeof(struct tht_param));
            iperf_cpu_freq = tls_reg_read32(HR_CLK_DIV_CTL)&0xFF;

            tls_os_queue_create(&tht_q, THT_QUEUE_SIZE);

            tls_os_task_create(NULL, NULL,
                               tht_task,
                               (void *)&gThtSys,
                               (void *)ThtTaskStk,
                               THT_TASK_STACK_SIZE * sizeof(u32),
                               THT_TASK_PRIO,
                               0);

            testing = 1;
            printf("CreateThroughputTask\n");
        }
#endif
    }
}

void tht_print_param(struct tht_param* tht)
{
	IPF_DBG("THT Parameters: \n");
	IPF_DBG("role: %c\n", tht->role);
	IPF_DBG("server_hostname: %s\n", tht->server_hostname);
	IPF_DBG("protocol: %d\n", tht->protocol);	
	IPF_DBG("report_interval: %d\n", tht->report_interval);	
	IPF_DBG("duration: %d\n", tht->duration);	
	IPF_DBG("rate: %llu\n", tht->rate);	
	IPF_DBG("block_size: %d\n", tht->block_size);	

}
int tht_parse_parameter(char *arg[])
{
	char* tmp;
	int len;
	struct tht_param* tht = (struct tht_param*)(&gThtSys);

	memset(tht, 0, sizeof(struct tht_param));

	switch (*arg[0]){
		case 'S':
		case 's':
			tht->role = 's';
			if((tmp = strchr(arg[1], '=')) != NULL) {
				tht->report_interval = atoi(tmp+1);
			}

			tht_print_param(tht);
		break;

		case 'C':
		case 'c':
			tht->role = 'c';

			len = arg[2] - arg[1] - 1	;
			MEMCPY(tht->server_hostname, arg[1], len);
			tht->server_hostname[len] = '\0';

			if(strcmp(arg[2], "TCP") == 0){
				tht->protocol = Ptcp;
				
				if((tmp = strchr(arg[3], '=')) != NULL) {
					tht->block_size = atoi(tmp+1);
				}
			}
			else if(strcmp(arg[2], "UDP") == 0){
				tht->protocol = Pudp;

				if((tmp = strchr(arg[3], '=')) != NULL) {
					tmp += 1;
					tht->rate = unit_atof(tmp);
				}
			}
			else{
				/* return protocol error*/
				return -1;
			}

			if((tmp = strchr(arg[4], '=')) != NULL) {
				tht->duration = atoi(tmp+1);
			}

			if((tmp = strchr(arg[5], '=')) != NULL) {
				tht->report_interval = atoi(tmp+1);
			}

			tht_print_param(tht);
		break;

		default:
			/* print help infor */
			return -1;
	}

	return 0;
	

}


int tht_start_iperf_test(char *arg[])
{
	int ret = 0;
	if (arg == NULL)
	{
		return -2;
	}

	CreateThroughputTask();

	ret = tht_parse_parameter(arg);
	if (ret < 0)
	{
		return -3;
	}	

	if (tht_q)
	{
		exit_last_test = 1;
		tls_os_queue_send(tht_q, (void *)TLS_MSG_WIFI_PERF_TEST_START, 0);
		return 0;
	}
	return -1;
}

#endif 
