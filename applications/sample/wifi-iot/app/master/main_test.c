#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "wm_mem.h"
#include "wm_ntp.h"
#include <libemqtt.h>
#include <cJSON.h>

#include "iot_gpio_neptune.h"
#include "iot_gpio_w800.h"
#include "iot_i2c.h"

#include "wifi_connecter.h"
#include "mpu6050.h"
#include "imu.h"
#include "fall_detect.h"

#include "max30102.h"   
#include "algorithm.h"
#include "myiic.h"

#define MQTT_DEMO_RECV_BUF_LEN_MAX      1024
#define MQTT_DEMO_READ_TIMEOUT        (-1000)  

#define MQTT_DEMO_READ_TIME_SEC         1      //时延秒
#define MQTT_DEMO_READ_TIME_US          0      //微秒

#define MQTT_DEMO_CLIENT_ID            "CQUPTLEI_CQUPTLEI_0_0_2022050513"                                      // client id,  这三个参数需要用注册设备后自动保存的账号密码到下面的网站去自动生成
#define MQTT_DEMO_DEVICE_ID            "CQUPTLEI_CQUPTLEI"                                                     // username，  网址：https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/
#define MQTT_DEMO_PASSWORD             "43720cfe8af19d99685c657408dbd0bb10e204ce3d16c87bfdf5124535d8ab82"      // password                                                      

#define MQTT_DEMO_SUB_TOPIC            "$oc/devices/CQUPTLEI_CQUPTLEI/sys/commands/#"                          //设备侧订阅主题：$oc/devices/{device_id}/sys/commands/#
#define MQTT_DEMO_PUB_TOPIC            "$oc/devices/CQUPTLEI_CQUPTLEI/sys/properties/report"                   //发布主题，设备属性上报：$oc/devices/{device_id}/sys/properties/report
#define MQTT_DEMO_ACK_TOPIC            "$oc/devices/CQUPTLEI_CQUPTLEI/sys/commands/response/"                  //设备返回命令响应:$oc/devices/{device_id}/sys/commands/response/request_id={request_id}

#define MQTT_DEMO_SERVER_ADDR          "iot-mqtts.cn-north-4.myhuaweicloud.com"                                //华为云服务器地址与端口号（mqtts为8883），不用修改
#define MQTT_DEMO_SERVER_PORT           1883    

static uint8_t pcaket_buffer[MQTT_DEMO_RECV_BUF_LEN_MAX];
static int socket_id;                         //套接字id
static int mqtt_keepalive = 60;               //持续连接，1分钟发送一次心跳连接
static mqtt_broker_handle_t mqtt_broker;      //（MQTT是一个TCP服务端,称为broker）
static osTimerId_t id1,id2;                   //定义2个定时器
                                              //osTimerStart (osTimerId_t timer_id, uint32_t ticks);启动定时器，定时器id，超时时间


MPU6050_Data_TypeDef MPU6050_Data;

//MPU6050获取参数及处理参数
int Gyro_y=0,Gyro_x=0,Gyro_z=0;           //Y轴陀螺仪数据暂存
int Accel_x=0,Accel_y=0,Accel_z=0;	      //X轴加速度值暂存
float  Angle_ax=0.0,Angle_ay=0.0,Angle_az=0.0;  //由加速度计算的加速度(弧度制)
float  Angle_gy=0.0,Angle_gx=0.0,Angle_gz=0.0;  //由角速度计算的角速率(角度制)
int   g_x=0,g_y=0,g_z=0;            			//陀螺仪矫正参数
float a_x=0.0,a_y=0.0;  
float Pitch=0.0,Roll=0.0,Yaw=0.0;               //四元数解算出的欧拉角 


/***************************************************************
 * 说明：关爱老年人所需数据结构体
***************************************************************/
typedef struct 
{
    char acc_smv[20];       //合加速度
    char w_gyr[20];         //合角速度
    char angle[20];         //姿态角
    char timestamp[32];     //时间戳
    char fall[8];           //跌倒标志
    char heart_beat[20];    //心率
    char blood_density[20]; //血氧浓度
    char ON_OFF[8];         //led开关控制（测试用）
}Care_the_old_info;


/***************************************************************
 * 说明：mqtt状态结构体
***************************************************************/
typedef enum 
{
    MQTT_START=0,       //开启
    RECV_FROM_SERVER,   //收到消息
} MQTT_State_info;


Care_the_old_info Old_info;
MQTT_State_info now_state;


/***************************************************************
  * 函数功能:获取数据的时间戳
  * 输入参数: 无
  * 返 回 值: 老年人信息数据上传时候的时间戳
  * 说    明: 无
 ***************************************************************/ 
static int GetTimeStamp(void)
{
 struct tm *tblock;   
 uint32_t t = tls_ntp_client();         //从ntp服务器获取时间
 tblock = localtime((const time_t *)&t);
 sprintf(Old_info.timestamp,"%04d%02d%02d%s%02d%02d%02d%s",tblock->tm_year+1900,tblock->tm_mon+1, tblock->tm_mday,"T",tblock->tm_hour-8,tblock->tm_min,tblock->tm_sec,"Z");
}

/***************************************************************
  * 函数功能:关闭套接字
  * 输入参数: mqtt_broker结构体变量
  * 返 回 值: 成功返回0,否则返回SOCKET_ERROR错误
  * 说    明: socketid是broker的一个成员
 ***************************************************************/ 
static int close_socket(mqtt_broker_handle_t *broker)
{
    int fd = broker->socketid;
    return closesocket(fd);
}

/***************************************************************
  * 函数功能:发送数据
  * 输入参数: socketfd参数、要发送的数据指针、数据大小
  * 返 回 值: 成功返回发送的字节数，失败返回 -1
  * 说    明: 对于字节数大于一个字节的变量类型必须进行端序转换，char类型不用，0：阻塞发送
 ***************************************************************/ 
static int send_packet(int socket_info, const void *buf, unsigned int count)
{
    int fd = socket_info;
    return send(fd, buf, count, 0);
}

/***************************************************************
  * 函数功能:读取数据
  * 输入参数: 时延秒和微秒
  * 返 回 值:成功返回读取的字节数,错误返回-2,超时返回-1000，接收数据错误返回- 1
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
        // 如果超时，如果输入可用，则选择返回0，如果错误，则为1
        //socket_id + 1:集合中所有文件描述符的范围，即所有文件描述符的最大值加1:有一个文件可读，select就会返回一个大于0的值，表示有文件可读;
                         //如果没有可读的文件，则根据timeout参数再判断是否超时，若超出timeout的时间，select返回0，若发生错误返回负值
        //readfds:指向fd_set结构的指针，这个集合中应该包括文件描述符
        ret = select(socket_id + 1, &readfds, NULL, NULL, &tmv);
        if(ret < 0)
            return -2;
        else if(ret == 0)
            return MQTT_DEMO_READ_TIMEOUT;
    }

    int total_bytes = 0, bytes_rcvd, packet_length;
    memset(pcaket_buffer, 0, sizeof(pcaket_buffer)); //清空pcaket_buffer里面的内容
    //recv返回copy的字节数
    if((bytes_rcvd = recv(socket_id, (pcaket_buffer + total_bytes), MQTT_DEMO_RECV_BUF_LEN_MAX, 0)) <= 0)
    {
        //printf("%d, %d\r\n", bytes_rcvd, socket_id);
        return -1;
    }

    total_bytes += bytes_rcvd; // 保持总字节的计数
    if (total_bytes < 2)
        return -1;

    //总的数据长度包括协议头部等参数长度
    uint16_t rem_len = mqtt_parse_rem_len(pcaket_buffer);
    uint8_t rem_len_bytes = mqtt_num_rem_len_bytes(pcaket_buffer);

    packet_length = rem_len + rem_len_bytes + 1;

    while(total_bytes < packet_length) // 读数据包
    {
        if((bytes_rcvd = recv(socket_id, (pcaket_buffer + total_bytes), MQTT_DEMO_RECV_BUF_LEN_MAX, 0)) <= 0)
            return -1;
        total_bytes += bytes_rcvd; // 保持总字节的计数
    }

    return packet_length;
}

/***************************************************************
  * 函数功能: 套接字初始化
  * 输入参数: mqtt描述符、华为云mqtt服务器地址、端口号、持续连接时间
  * 返 回 值: 套接字创建失败返回 -1 ，链接失败返回-2， 成功返回0
  * 说    明: 无
 ***************************************************************/ 
static int init_socket(mqtt_broker_handle_t *broker, const char *hostname, short port, int keepalive)
{
    int flag = 1;
    struct hostent *hp;  //用于域名解析
    // 创建套接字
    if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    // 禁用Nagle算法，减小时延
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
  * 输入参数: 设备侧的订阅主题
  * 返 回 值: 成功返回0
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
  * 返 回 值: 成功返回0
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
    timerDelay = 60*500U;   //60s发送一次心跳消息
    if(osTimerStart(id1, timerDelay));
    {
      printf("  Timer Start\r\n");
    }

    timerDelay = 50U;  //0.1s上报一次，500U = 1s
    if(osTimerStart(id2, timerDelay));
    {
      printf("Publish_Topic Start\r\n");
    }

    return 0;
}

/***************************************************************
  * 函数功能:LED灯开关控制
  * 输入参数: 收到的控制命令
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int LED_OFF_ON(char *off_on)
{
    char on[]="ON";
    char off[]="OFF";
    
    memset(Old_info.ON_OFF,0,sizeof(Old_info.ON_OFF));
    
    if(strcmp(off_on,on)==0){
        IoTGpioSetOutputVal(IOT_GPIO_PB_08, IOT_GPIO_VALUE0);;//打开LED8灯
        sprintf(Old_info.ON_OFF,"%s","ON");
        printf(">>>LED灯打开<<<\n");
    }
    else if(strcmp(off_on,off)==0){
        IoTGpioSetOutputVal(IOT_GPIO_PB_08, IOT_GPIO_VALUE1);;//关闭LED8灯
        sprintf(Old_info.ON_OFF,"%s","OFF");
        printf(">>>LED灯关闭<<<\n");
    }
}

/***************************************************************
  * 函数功能:解析接收到的json数据
  * 输入参数: 数据
  * 返 回 值: 无
  * 说    明: 主要是云端给设备下达命令的解析
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
  * 说    明: 用于云下发命令后的响应
 ***************************************************************/ 
static int packPublishAck(char *jsonBuffer)
{
    cJSON *jsRet = NULL;
    int ackLen = 0; 
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
  * 函数功能:设备属性上报,封装成json格式
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
			cJSON_AddStringToObject(arrayObj_1, "service_id", "Care_the_old");

            cJSON *arrayObj_2 = cJSON_CreateObject();
            cJSON_AddItemToObject(arrayObj_1, "properties", arrayObj_2);
            
            cJSON_AddStringToObject(arrayObj_2, "acc_smv", Old_info.acc_smv);
            cJSON_AddStringToObject(arrayObj_2, "w_gyr", Old_info.w_gyr);
            cJSON_AddStringToObject(arrayObj_2, "angle", Old_info.angle);
            cJSON_AddStringToObject(arrayObj_2, "fall", Old_info.fall);
            cJSON_AddStringToObject(arrayObj_2, "heart_beat", Old_info.heart_beat);
            cJSON_AddStringToObject(arrayObj_2, "blood_density", Old_info.blood_density);

            cJSON_AddStringToObject(arrayObj_1,"event_time", Old_info.timestamp);
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
  * 函数功能:发布topic
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
static int publish_topic(void)
{
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
  * 函数功能:获取老人健康信息
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void Get_Old_info(void)
{
    float acc=3.0,w=2.0,angle=40.0,blood=99.6;
    int fall=1,heart=60;

    //osDelay(5);                     //延时10ms

    MPU6050_Read_Data();            //获取数据
    Accel_y= MPU6050_Data.Accel[0];	//读取6050加速度数据
	Accel_x= MPU6050_Data.Accel[1] ;		   
	Accel_z= MPU6050_Data.Accel[2] ;

    Gyro_x = MPU6050_Data.Gyro[0]-g_x;//读取6050角速度数据
	Gyro_y = MPU6050_Data.Gyro[1]-g_y;
	Gyro_z = MPU6050_Data.Gyro[2]-g_z;	

    Angle_ax = Accel_x/8192.0;        //根据设置换算
	Angle_ay = Accel_y/8192.0;
	Angle_az = Accel_z/8192.0;
	
	Angle_gx = Gyro_x/65.5*0.0174533;//根据设置换算
	Angle_gy = Gyro_y/65.5*0.0174533;
	Angle_gz = Gyro_z/65.5*0.0174533;

    IMUupdate(Angle_gx,Angle_gy,Angle_gz,Angle_ax,Angle_ay,Angle_az);	

    fall=suspect_fall_detect(Angle_gx,Angle_gy,Angle_gz,Angle_ax,Angle_ay,Angle_az,Roll,Yaw);
    acc = acc_smv_cul(Angle_ax,Angle_ay, Angle_az);
    // acc=fabs(Angle_ax)+fabs(Angle_ay)+fabs(Angle_az);
    w = w_gyr_cul(Angle_gx,Angle_gy,Angle_gz);
    angle = pose_cul(Roll,Pitch);

	    printf("Roll:%.2f",Roll);			  //输出横滚角,绕z轴 
        printf("    Yaw:%.2f",Yaw);			  //y
        printf("    Pitch:%.2f",Pitch);	  //x
        printf("    Accel_x:%.4f",Angle_ax);
        printf("    Accel_y:%.4f",Angle_ay);
        printf("    Accel_z:%.4f",Angle_az);
        printf("    Angle_x:%.2f",Angle_gx);
        printf("    Angle_y:%.2f",Angle_gy);
        printf("    Angle_z:%.2f",Angle_gz);
        printf("    ACC_smv:%.2f",acc);
        printf("    w_gyr:%.2f",w);
        printf("    Angle:%.2f",angle);
        printf("    Fall:%d\n",fall);

    sprintf(Old_info.acc_smv,"%.2f",acc);
    sprintf(Old_info.w_gyr,"%.2f",w);
    sprintf(Old_info.angle,"%.2f",angle);
    sprintf(Old_info.fall,"%d",fall);
    sprintf(Old_info.heart_beat,"%d",heart);
    sprintf(Old_info.blood_density,"%.2f",blood);
}

/***************************************************************
  * 函数功能:网络检查
  * 输入参数: 无
  * 返 回 值: 连接成功返回0
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
static void MQTT_Task(void)
{
    IIC_Init();
	max30102_init();
    osDelay(10);
    MPU6050_GPIO_Init();                 //MPU6050初始化
    osDelay(50);
    printf("================================================================================\n");
    if (!isWifiNetworkOk())
    {
        printf("===============================WIFI ONLine===============================\r\n");
    }
    else
    {
        printf("===============================WIFI OFFLine===============================\r\n");
    }

    printf("  \n Start MQTT  \r\n");
    while (1)
    {
        // Get_Old_info();
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

/***************************************************************
 *函数功能：  定时器回调函数
 *说明：     Keep Alive指定连接最大空闲时间T，当客户端检测到连接空闲时间超过T时，
 *          必须向Broker发送心跳报文PINGREQ，Broker收到心跳请求后返回心跳响应PINGRESP。
 *          若Broker超过1.5T时间没收到心跳请求则断开连接，并且投递遗嘱消息到订阅方；
 *          同样，若客户端超过一定时间仍没收到心跳响应PINGRESP则断开连接。 
****************************************************************/
void Timer_Callback1(void *arg)
{
    (void)arg;
    printf("Send Heart Ping\r\n");
    mqtt_ping(&mqtt_broker);
}

/***************************************************************
  * 函数功能:定时器回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 获取温度、获取时间戳、发布topic
 ***************************************************************/ 
void Timer_Callback2(void *arg)
{
    (void)arg;
    Get_Old_info();
    GetTimeStamp();
    publish_topic(); 
    //printf("Timer 2 \n\n");
}

/***************************************************************
  * 函数功能:创建定时器任务
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: osTimerNew（回调函数，连续运行，参数，属性）
 ***************************************************************/ 
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
    //osThreadAttr_t是osThreadNew的一个参数，这里创建了2个线程，参数相同
    osThreadAttr_t attr;
 
    IoTGpioInit(IOT_GPIO_PB_08);
    IoTGpioSetDir(IOT_GPIO_PB_08,IOT_GPIO_DIR_OUT);

    if (IoTI2cInit(0, 200*1000)) {
       printf("I2C Init Failed\n");
    }
    attr.name = "MQTT_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)MQTT_Task, NULL, &attr) == NULL) {
        printf("[MQTT_OC_Demo] Falied to create MQTT_Task!\n");
    }

    if (osThreadNew((osThreadFunc_t)Timer_RunTask, NULL, &attr) == NULL) {
        printf("[Timer_RunTask] Falied to create Timer_RunTask!\n");
    } 
    
}
APP_FEATURE_INIT(MQTT_OC_Demo);