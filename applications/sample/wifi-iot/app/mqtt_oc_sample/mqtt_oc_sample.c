#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "aht21.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

#include "wifi_connecter.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "wm_mem.h"
#include "wm_ntp.h"
#include <libemqtt.h>
#include <cJSON.h>

#include "iot_gpio_neptune.h"
#include "iot_gpio_w800.h"
#include "iot_i2c.h"

#define MQTT_DEMO_RECV_BUF_LEN_MAX      1024
#define MQTT_DEMO_READ_TIMEOUT        (-1000)

#define MQTT_DEMO_READ_TIME_SEC         1
#define MQTT_DEMO_READ_TIME_US          0

#define MQTT_DEMO_CLIENT_ID            "616268529fff74057ddd731b_202110101314_0_0_2021101006"                        //client id
#define MQTT_DEMO_DEVICE_ID            "616268529fff74057ddd731b_202110101314"                                        // device id
#define MQTT_DEMO_PASSWORD             "b6fd9631cd69eee9ce565a36564b93d26760a49ace05be96cbe9dfaab91f275d"              //password
#define MQTT_DEMO_SUB_TOPIC            "$oc/devices/616268529fff74057ddd731b_202110101314/sys/commands/#"             //订阅主题
#define MQTT_DEMO_PUB_TOPIC            "$oc/devices/616268529fff74057ddd731b_202110101314/sys/properties/report"      //发布主题
#define MQTT_DEMO_ACK_TOPIC            "$oc/devices/616268529fff74057ddd731b_202110101314/sys/commands/response/"

#define MQTT_DEMO_SERVER_ADDR          "iot-mqtts.cn-north-4.myhuaweicloud.com"        //华为云服务器地址与端口号（mqtt为8883）
#define MQTT_DEMO_SERVER_PORT           1883    

static uint8_t pcaket_buffer[MQTT_DEMO_RECV_BUF_LEN_MAX];
static int socket_id;
static int mqtt_keepalive = 60;               //持续连接
static mqtt_broker_handle_t mqtt_broker;      //（MQTT是一个TCP服务端,称为broker）
static osTimerId_t id1,id2;
//osTimerStart (osTimerId_t timer_id, uint32_t ticks);启动定时器，定时器id，超时时间

typedef struct 
{
    char temp[20];
    char humi[20];
    char timestamp[32];
    char ON_OFF[8];
}Temperature_info;

typedef enum 
{
    MQTT_START=0,
    RECV_FROM_SERVER,
} MQTT_State_info;

Temperature_info Temperature;
MQTT_State_info now_state;



/***************************************************************
  * 函数功能:获取数据的时间戳
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int GetTimeStamp(void)
{
 struct tm *tblock;   
 uint32_t t = tls_ntp_client(); //获取时间
 tblock = localtime((const time_t *)&t);
 sprintf(Temperature.timestamp,"%04d%02d%02d%s%02d%02d%02d%s",tblock->tm_year+1900,tblock->tm_mon+1, tblock->tm_mday,"T",tblock->tm_hour-8,tblock->tm_min,tblock->tm_sec,"Z");
}

/***************************************************************
  * 函数功能:关闭套接字
  * 输入参数: mqtt对象指针
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int close_socket(mqtt_broker_handle_t *broker)
{
    int fd = broker->socketid;
    return closesocket(fd);
}

/***************************************************************
  * 函数功能:发送包
  * 输入参数: 
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int send_packet(int socket_info, const void *buf, unsigned int count)
{
    int fd = socket_info;
    return send(fd, buf, count, 0);
}

/***************************************************************
  * 函数功能:读取包
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int read_packet(int sec, int us)
{
    int ret = 0;

    if ((sec >= 0) || (us >= 0))
    {
        fd_set readfds;
        struct timeval tmv;

        // 初始化文件描述符集
        FD_ZERO (&readfds);
        FD_SET (socket_id, &readfds);

        // 初始化超时数据结构
        tmv.tv_sec = sec;
        tmv.tv_usec = us;

        // 如果超时，1如果输入可用，则选择返回0，如果错误，则为1
        ret = select(socket_id + 1, &readfds, NULL, NULL, &tmv);
        if(ret < 0)
            return -2;
        else if(ret == 0)
            return MQTT_DEMO_READ_TIMEOUT;

    }

    int total_bytes = 0, bytes_rcvd, packet_lengthgthgth;
    memset(pcaket_buffer, 0, sizeof(pcaket_buffer));

    if((bytes_rcvd = recv(socket_id, (pcaket_buffer + total_bytes), MQTT_DEMO_RECV_BUF_LEN_MAX, 0)) <= 0)
    {
        //printf("%d, %d\r\n", bytes_rcvd, socket_id);
        return -1;
    }

    total_bytes += bytes_rcvd; // 保持总字节的计数
    if (total_bytes < 2)
        return -1;


    uint16_t rem_len = mqtt_parse_rem_len(pcaket_buffer);
    uint8_t rem_len_bytes = mqtt_num_rem_len_bytes(pcaket_buffer);

    packet_lengthgthgth = rem_len + rem_len_bytes + 1;

    while(total_bytes < packet_lengthgthgth) // 读数据包
    {
        if((bytes_rcvd = recv(socket_id, (pcaket_buffer + total_bytes), MQTT_DEMO_RECV_BUF_LEN_MAX, 0)) <= 0)
            return -1;
        total_bytes += bytes_rcvd; // 保持总字节的计数
    }

    return packet_lengthgthgth;
}

/***************************************************************
  * 函数功能:套接字初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int init_socket(mqtt_broker_handle_t *broker, const char *hostname, short port, int keepalive)
{
    int flag = 1;
    struct hostent *hp;

    // 创建套接字
    if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // 禁用Nagle算法
    if (setsockopt(socket_id, IPPROTO_TCP, 0x01, (char *)&flag, sizeof(flag)) < 0)
    {
        close_socket(&mqtt_broker);
        return -2;
    }

    // 查询主机IP启动
    hp = gethostbyname(hostname);
    if (hp == NULL )
    {
        close_socket(&mqtt_broker);
        return -2;
    }

    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(struct sockaddr_in));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    memcpy(&(socket_address.sin_addr), hp->h_addr, hp->h_length);

    // 连接套接字
    if((connect(socket_id, (struct sockaddr *)&socket_address, sizeof(socket_address))) < 0)
    {
        close_socket(&mqtt_broker);
        return -1;
    }

    // MQTT stuffs
    mqtt_set_alive(broker, mqtt_keepalive);
    broker->socketid = socket_id;
    broker->mqttsend = send_packet;
    return 0;
}

/***************************************************************
  * 函数功能:订阅主题
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int subscribe_topic(char *topic)
{
    unsigned short msg_id = 0, msg_id_rcv = 0;
    int packet_lengthgth = 0;
	int ret = -1;
 
    if(topic == NULL) {
        return -1;
    }
    
    ret = mqtt_subscribe(&mqtt_broker, topic, &msg_id);
	if( ret == -1 ) {
		close_socket(&mqtt_broker);
		return -1;
	}
    packet_lengthgth = read_packet(MQTT_DEMO_READ_TIME_SEC, MQTT_DEMO_READ_TIME_US);
    if(packet_lengthgth < 0)
    {
        printf("Error(%d) on read packet!\n", packet_lengthgth);
        close_socket(&mqtt_broker);
        return -1;
    }
 
    if(MQTTParseMessageType(pcaket_buffer) != MQTT_MSG_SUBACK)
    {
        printf("SUBACK expected!\n");
        close_socket(&mqtt_broker);
        return -2;
    }
 
    msg_id_rcv = mqtt_parse_msg_id(pcaket_buffer);
	if(msg_id != msg_id_rcv)
	{
		printf("%d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv);
        close_socket(&mqtt_broker);
        return -3;
	}
 
    return 0;
}

/***************************************************************
  * 函数功能:MQTT初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: mqtt初始化--->建立TCP连接--->建立mqtt连接--->订阅mqtt
 ***************************************************************/ 
static int MQTT_Init(void)
{
    int packet_lengthgthgth, ret = 0;
    uint16_t msg_id, msg_id_rcv;

    printf("  step1: Init MQTT Lib\r\n");              //步骤1:MQTT初始化
    mqtt_init(&mqtt_broker, MQTT_DEMO_CLIENT_ID);

    printf("  step2: Establishing TCP Connection\r\n");//步骤2:建立TCP连接
    mqtt_init_auth(&mqtt_broker,MQTT_DEMO_DEVICE_ID,MQTT_DEMO_PASSWORD);
    ret = init_socket(&mqtt_broker,MQTT_DEMO_SERVER_ADDR, MQTT_DEMO_SERVER_PORT, mqtt_keepalive);
    if(ret)
    {
        printf("init_socket ret=%d\n", ret);
        return -4;
    }

    printf("  step3: Establishing MQTT Connection\r\n");//步骤3:建立MQTT连接
    ret = mqtt_connect(&mqtt_broker);
    if(ret)
    {
        printf("mqtt_connect ret=%d\n", ret);
        return -5;
    }

    packet_lengthgthgth = read_packet(MQTT_DEMO_READ_TIME_SEC, MQTT_DEMO_READ_TIME_US);
    if(packet_lengthgthgth < 0)
    {
        printf("Error(%d) on read packet!\n", packet_lengthgthgth);
        close_socket(&mqtt_broker);
        return -1;
    }
    //printf("0x%02x\n",pcaket_buffer[3]);
    if(MQTTParseMessageType(pcaket_buffer) != MQTT_MSG_CONNACK||pcaket_buffer[3] != 0x00)
    {
        printf("CONNACK expected or failed!\n");
        close_socket(&mqtt_broker);
        return -2;
    }

    printf("  step4: Subscribe MQTT\r\n");                    //步骤4：订阅MQTT
    if(subscribe_topic(MQTT_DEMO_SUB_TOPIC) != 0) {
        return -3;
    }
    printf("  MQTT Connect & subscribe topic success\n");

    uint32_t timerDelay;
    timerDelay = 60*500U;  
    if(osTimerStart(id1, timerDelay));
    {
      printf("  Timer Start\r\n");
    }

    timerDelay = 10*500U;
    if(osTimerStart(id2, timerDelay));
    {
      printf("Publish_Topic Start\r\n");
    }

    return 0;
}

/***************************************************************
  * 函数功能:LED灯开关控制
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int LED_OFF_ON(char *off_on)
{
    char on[]="ON";
    char off[]="OFF";
    
    memset(Temperature.ON_OFF,0,sizeof(Temperature.ON_OFF));
    if(strcmp(off_on,on)==0){
        IoTGpioSetOutputVal(IOT_GPIO_PB_08, IOT_GPIO_VALUE0);;//打开LED8灯
        sprintf(Temperature.ON_OFF,"%s","ON");
        printf(">>>LED灯打开<<<\n");
    }
    else if(strcmp(off_on,off)==0){
        IoTGpioSetOutputVal(IOT_GPIO_PB_08, IOT_GPIO_VALUE1);;//关闭LED8灯
        sprintf(Temperature.ON_OFF,"%s","OFF");
        printf(">>>LED灯关闭<<<\n");
    }
}

/***************************************************************
  * 函数功能:解析接收到的数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int parseReceivedData(uint8_t *data)
{
    cJSON *json = NULL;
	cJSON *paras,*Value,*service_id,*command_name;
	
	json = cJSON_Parse(data);
	if(json)
	{
        paras = cJSON_GetObjectItem(json,"paras");
        Value = cJSON_GetObjectItem(paras,"Value");
		service_id = cJSON_GetObjectItem(json,"service_id");
		command_name = cJSON_GetObjectItem(json,"command_name");
        LED_OFF_ON(Value->valuestring);
		cJSON_Delete(json); 
	}
	else
	{
		printf("parse json error!\r\n");
	}
}

/***************************************************************
  * 函数功能:打包发布应答
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int packPublishAck(char *jsonBuffer)
{
    cJSON *jsRet = NULL;
    int ackLen = 0; 计算三轴合角速度
    {
    cJSON_AddStringToObject(jsRet,"result_code","0");
    cJSON_AddStringToObject(jsRet,"response_name","led_control");
    cJSON *jsArray=cJSON_CreateObject();
    cJSON_AddItemToObject(jsRet, "paras", jsArray);
    cJSON_AddStringToObject(jsArray, "result", "success");
    
    char *databuf = cJSON_PrintUnformatted(jsRet);
		if(databuf) {
			if( jsonBuffer ) {
				ackLen = strlen(databuf);
				memcpy(jsonBuffer, databuf,ackLen);
			}
            tls_mem_free(databuf);
        }
        cJSON_Delete(jsRet); 
    }
    return ackLen;
}

/***************************************************************
  * 函数功能:打包发布请求
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int packPublishReq(char *jsonBuffer)
{
	cJSON *jsRet = NULL;
	cJSON *jsArray = NULL;
	int ackLen = 0;
	
	jsRet = cJSON_CreateObject();
	if(jsRet)
	{   
		jsArray = cJSON_CreateArray();
        cJSON_AddItemToObject(jsRet, "services", jsArray);
        {
            cJSON *arrayObj_1 = cJSON_CreateObject();
            cJSON_AddItemToArray(jsArray, arrayObj_1);
			cJSON_AddStringToObject(arrayObj_1, "service_id", "Temperature");

            cJSON *arrayObj_2 = cJSON_CreateObject();
            cJSON_AddItemToObject(arrayObj_1, "properties", arrayObj_2);
            cJSON_AddStringToObject(arrayObj_2, "temp", Temperature.temp);
            cJSON_AddStringToObject(arrayObj_2, "humi", Temperature.humi);
            cJSON_AddStringToObject(arrayObj_2, "led",  Temperature.ON_OFF);
            
            cJSON_AddStringToObject(arrayObj_1,"event_time", Temperature.timestamp);
        }
        char *databuf = cJSON_PrintUnformatted(jsRet);
		if(databuf) {
			if( jsonBuffer ) {
				ackLen = strlen(databuf);
				memcpy( jsonBuffer, databuf,ackLen);
			}
            tls_mem_free(databuf);
        }
        cJSON_Delete(jsRet); 
    }
    return ackLen;
}

/***************************************************************
  * 函数功能:发布主题
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int publish_topic(void)
	char *ackBuffer = NULL;
	int ackLen = 0;
 
	ackBuffer = tls_mem_alloc(MQTT_DEMO_RECV_BUF_LEN_MAX);
	if( ackBuffer )
	{
		memset(ackBuffer,0,MQTT_DEMO_RECV_BUF_LEN_MAX);
		ackLen = packPublishReq(ackBuffer);
		mqtt_publish(&mqtt_broker,(const char *)MQTT_DEMO_PUB_TOPIC,ackBuffer,ackLen, 0);
		tls_mem_free(ackBuffer);
	}
	return ackLen;
}

/***************************************************************
  * 函数功能:mqtt接收数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int MQTT_Recv(void)
{
    int packet_lengthgth = 0,ret=0;
    packet_lengthgth = read_packet(0, 1);
    if(packet_lengthgth == MQTT_DEMO_READ_TIMEOUT)
    {
        return MQTT_DEMO_READ_TIMEOUT;
    }
    else if(packet_lengthgth > 0)
    {
        if (MQTTParseMessageType(pcaket_buffer) == MQTT_MSG_PUBLISH)
        {
            uint8_t topic[64+16] = { 0 },request_id[48]={0},ACK_TOPIC[128]={0}, *msg;
			uint16_t len;
			int32_t midValue = 0, ackLen = 0;
			char *ackBuffer = NULL;
            len = mqtt_parse_pub_topic(pcaket_buffer, topic);
			topic[len] = '\0';
			len = mqtt_parse_publish_msg(pcaket_buffer, &msg);
            strncpy(request_id,topic+63,47);
            sprintf(ACK_TOPIC,"%s%s",MQTT_DEMO_ACK_TOPIC,request_id);
            parseReceivedData(msg);
            ackBuffer = tls_mem_alloc(MQTT_DEMO_RECV_BUF_LEN_MAX);
            if( ackBuffer )
            {
 
                memset(ackBuffer, 0, MQTT_DEMO_RECV_BUF_LEN_MAX);
				ackLen = packPublishAck(ackBuffer);
				mqtt_publish(&mqtt_broker, ACK_TOPIC, ackBuffer, ackLen, 0);
				tls_mem_free(ackBuffer);   
            }
            else
            {
            return 1;
            }
        }
        else if(ret == MQTT_MSG_PINGRESP) {
            printf("recv pong\n");
        }
        else {
            printf("Packet Header: 0x%x\n", pcaket_buffer[0]);
        }
         return 0;  
    }
    else if(packet_lengthgth == -1)
    {
    	printf("packet_lengthgth:%d\n", packet_lengthgth);
        return -1;
    }
    return 0;
}

/***************************************************************
  * 函数功能:获取温度
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void GetTemperature(void)
{
    float temp = 0.0, humi = 0.0;
    AHT21_StartMeasure();
    AHT21_GetMeasureResult(&temp, &humi);
    sprintf(Temperature.temp,"%.2f",temp);
    sprintf(Temperature.humi,"%.2f",humi);
}

/***************************************************************
  * 函数功能:网络检查
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int isWifiNetworkOk(void)
{
    WifiDeviceConfig config = {0};

	strcpy(config.ssid, PARAM_HOTSPOT_SSID);
    strcpy(config.preSharedKey, PARAM_HOTSPOT_PSK);
    config.securityType = PARAM_HOTSPOT_TYPE;
    return ConnectToHotspot(&config);
}

//================================================================================================================================//
static void MQTT_OC_DemoTask(void)
{
    AHT21_Calibrate();
    osDelay(50);
    if (isWifiNetworkOk())
    {
        printf("  WIFI ONLine \r\n");
    }
    else
    {
        printf("  WIFI OFFLine  \r\n");
    }

    printf("  Start MQTT  \r\n");
    while (1)
    {
        switch (now_state)
        {
        case MQTT_START:
            do
            {
              if(MQTT_Init())
                break;  
              now_state=RECV_FROM_SERVER;   
            }
            while (0);
            break;
        case RECV_FROM_SERVER:
            MQTT_Recv();
            break; 
        default:
            break;
        }
    }
}

void Timer_Callback1(void *arg)
{
    (void)arg;
    printf("Send Heart Ping\r\n");
    mqtt_ping(&mqtt_broker);
}

void Timer_Callback2(void *arg)
{
    (void)arg;
    GetTemperature();
    GetTimeStamp();
    publish_topic(); 
}

static void Timer_RunTask(void)
{ 
    uint32_t exec1,exec2;                           
    exec1 = 1U;
    exec2 = 1U;
    id1 = osTimerNew(Timer_Callback1, osTimerPeriodic, &exec1, NULL);
    id2 = osTimerNew(Timer_Callback2, osTimerPeriodic, &exec2, NULL);
}

static void MQTT_OC_Demo(void)
{
    osThreadAttr_t attr;
 
    IoTGpioInit(IOT_GPIO_PB_08);
    IoTGpioSetDir(IOT_GPIO_PB_08,IOT_GPIO_DIR_OUT);
    if (IoTI2cInit(0, 200*1000)) {
        printf("AHT21 Test I2C Init Failed\n");
    }
    attr.name = "MQTT_OC_DemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)MQTT_OC_DemoTask, NULL, &attr) == NULL) {
        printf("[MQTT_OC_Demo] Falied to create MQTT_OC_DemoTask!\n");
    }

    if (osThreadNew((osThreadFunc_t)Timer_RunTask, NULL, &attr) == NULL) {
        printf("[Timer_RunTask] Falied to create Timer_RunTask!\n");
    } 
    
}
APP_FEATURE_INIT(MQTT_OC_Demo);