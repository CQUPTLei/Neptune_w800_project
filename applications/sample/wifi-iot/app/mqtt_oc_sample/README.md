# OpenHarmony Neptune开发板-MQTT连接华为IoT平台

本示例将演示如何在Neptune开发板上使用MQTT协议连接华为IoT平台,使用的是ATH20温湿度传感器模块与Neptune开发板

本示例实现AHT20温湿度数据上报华为IoT平台,IoT平台下发命令控制LED灯的开关

使用W800 SDK功能包中libmqtt来实现连接华为IoT平台

## 程序设计

### 初始化

### 一、MQTT初始化

> void mqtt_init(mqtt_broker_handle_t* broker, const char* clientid);

初始化要连接到代理的信息

| **参数** | **描述**                     |
| :------------- | :--------------------------------- |
| broker         | 代理数据结构，包含与代理的连接信息 |
| clientId       | clientId标识客户端ID               |
| **返回** | **描述**                     |
| 无             | 无                                 |

### 二、写入username与password

> void mqtt_init_auth(mqtt_broker_handle_t* broker, const char* username, const char* password);

启用身份验证以连接到代理。

| **参数** | **描述**                     |
| :------------- | :--------------------------------- |
| broker         | 代理数据结构，包含与代理的连接信息 |
| username       | 用户名                             |
| password       | 密码                               |
| **返回** | **描述**                     |
| 无             | 无                                 |

### 三、建立TCP连接

#### 编写TCP连接函数,代码示例如下：

```c
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
```

### 四、建立MQTT连接

> int mqtt_connect(mqtt_broker_handle_t* broker);

| **参数** | **描述**                     |
| :------------- | :--------------------------------- |
| broker         | 代理数据结构，包含与代理的连接信息 |
| **返回** | **描述**                     |
| 1              | 成功                               |
| 0              | 连接错误                           |
| -1             | 输入输出错误                       |

### 五、订阅MQTT

编写订阅MQTT主题函数,代码示例如下：

```c
static int subscribe_topic(char *topic)//订阅主题
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
```

### 数据推送与解析

采用cJSON封包与解包(使用W800 SDK功能包中cJSON实现),共有两个封包(一个设备属性上报,一个命令应答上报),一个解包解析IoT平台命令,其他不过多赘述具体详见华为IoTDA 设备接入文档: [设备接入 IoTDA 文档](https://support.huaweicloud.com/api-iothub/iot_06_v5_3002.html)

例如设备属性上报，代码示例如下:

```c
static int packPublishReq(char *jsonBuffer)//打包发布请求
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
```

### 华为IoT平台配置(以导出模型model.zip直接导入即可）

#### 登录

设备接入华为云平台之前，需要在平台注册用户账号，华为云地址：[https://www.huaweicloud.com/](https://www.huaweicloud.com/)

在华为云首页单击产品，找到IoT物联网，单击设备接入IoTDA 并单击立即使用。

![](https://images.gitee.com/uploads/images/2021/0625/205553_43ed6813_8343063.png "1.png")

![](https://images.gitee.com/uploads/images/2021/0625/205651_3fa9d867_8343063.png "2.png")

### 创建产品

在设备接入页面可看到总览界面，展示了华为云平台接入的协议与域名信息，根据需要选取MQTT通讯必要的信息备用。

接入协议（端口号）：MQTT 1883

域名：iot-mqtts.cn-north-4.myhuaweicloud.com

选中侧边栏产品页，单击右上角“创建产品”
![](https://images.gitee.com/uploads/images/2021/0625/205725_31e4a534_8343063.png "3.png")

在页面中选中所属资源空间，并且按要求填写产品名称，选中MQTT协议，数据格式为JSON，并填写厂商名称，在下方模型定义栏中选择所属行业以及添加设备类型，并单击右下角“立即创建”如图：
![](https://images.gitee.com/uploads/images/2021/0625/205744_06ad7a6a_8343063.png "4.png")

创建完成后，在产品页会自动生成刚刚创建的产品，单击“查看”可查看创建的具体信息。
![](https://images.gitee.com/uploads/images/2021/0625/205759_667fde8e_8343063.png "5.png")

单击产品详情页的自定义模型，在弹出页面中新增服务

服务ID：Temperature(必须一致)

服务类型：senser(可自定义)
![](https://images.gitee.com/uploads/images/2021/0625/205814_84a3b142_8343063.png "6.png")

在“Temperature”的下拉菜单下点击“添加属性”填写相关信息“temp”，“humi”，“led”。
![](https://images.gitee.com/uploads/images/2021/0625/205827_936710b3_8343063.png "7.png")

![](https://images.gitee.com/uploads/images/2021/0625/205840_d8661a2d_8343063.png "8.png")

在“Temperature”的下拉菜单下点击“添加命令”填写相关信息。
![](https://images.gitee.com/uploads/images/2021/0625/205852_0788731f_8343063.png "9.png")

#### 注册设备

在侧边栏中单击“设备”，进入设备页面，单击右上角“注册设备”，勾选对应所属资源空间并选中刚刚创建的产品，注意设备认证类型选择“秘钥”，按要求填写秘钥。
![](https://images.gitee.com/uploads/images/2021/0625/205909_44bc8e44_8343063.png "10.png")
![](https://images.gitee.com/uploads/images/2021/0625/205929_e2a7f796_8343063.png "11.png")

#### 修改代码中设备信息

在连接平台前需要获取CLIENT_ID、USERNAME、PASSWORD[这里](https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/)，访问，填写注册设备时生成的设备ID和设备密钥生成连接信息（ClientId、Username、Password），并将修改代码对应位置。
![](https://images.gitee.com/uploads/images/2021/0625/220102_38e48023_8343063.png "12.png")
![](https://images.gitee.com/uploads/images/2021/0625/220117_37f8c365_8343063.png "13.png")

在wifi_connecter.h修改wifi热点信息
![](https://images.gitee.com/uploads/images/2021/0625/220131_c335e6d3_8343063.png "14.png")

示例代码编译烧录代码后，按下开发板的RESET按键:

![](https://images.gitee.com/uploads/images/2021/1011/105711_f73786ec_8343063.png "屏幕截图 2021-10-11 102603.png")

点击设备右侧的“查看”，进入设备详情页面，可看到上报的数据
![](https://images.gitee.com/uploads/images/2021/1011/105617_d6fa858e_8343063.jpeg "Inked屏幕截图 2021-10-11 102215_LI.jpg")
在华为云平台设备详情页，单击“命令”，选择同步命令下发，选中创建的命令属性，单击“确定”，即可发送下发命令控制设备
![](https://images.gitee.com/uploads/images/2021/0625/220230_b8d2de76_8343063.png "17.png")
![](https://images.gitee.com/uploads/images/2021/1011/105552_3adb62d3_8343063.png "屏幕截图 2021-10-11 102300.png")

#### **注意：！！！需要将libemqtt.h下！！！**

```c
MQTT_CONF_USERNAME_LENGTH 修改为64
MQTT_CONF_PASSWORD_LENGTH 修改为64+8
clientid[50]修改为clientid[64]
```
