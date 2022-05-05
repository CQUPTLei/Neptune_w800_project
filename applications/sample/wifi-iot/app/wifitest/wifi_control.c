/*
版本：修改版，使用开发版的PB9按键控制wifi的连接与断开
说明：Neptune w800作为接入设备（STA）连接wifi（AP），WIFI名称和密码需要在程序中配置
知识点： AP:     也就是无线接入点，是一个无线网络的创建者，是网络的中心节点。一般家庭或办公室使用的无线路由器就一个AP。
        STA站点: 每一个连接到无线网络中的终端(如笔记本电脑、PDA及其它可以联网的用户设备)都可称为一个站点。
运行流程：
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"            //Provides functions for the Wi-Fi station and hotspot modes.即wif连接、断开、参数配置等功能

#include "lwip/netifapi.h"
// #include "lwip/api_shell.h"

#include "iot_gpio_w800.h"        //定义了w800引脚，例：IOT_GPIO_PA_00 为PA0
#include "iot_gpio_neptune.h"     //提供了操作设备的API：flash, GPIO, I2C, PWM, UART, and watchdog APIs.可进入查看函数简介、参数、返回等信息

#define LOG_TAG "Wifista: "    //日志标签
#define LOGI(fmt, ...) printf("[%d] INFO %s " fmt "\r\n", osKernelGetTickCount(), LOG_TAG, ##__VA_ARGS__)  //变参log打印

static bool control_flag = 0;    //按键控制标志

static void PrintLinkedInfo(WifiLinkedInfo* info)   //打印连接信息，在bssid上右键即可打开对应的头文件，查看具体的参数含义
{
    if (!info) return;
    static char macAddress[32] = {0};    //暂时存疑，mac地址是6个字节，即6个char，此处为什么用32个char呢？
    unsigned char* mac = info->bssid;    //在基础设施BSS中,BSSID是无线接入点(WAP)的MAC地址.
    snprintf(macAddress, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X",  //格式化mac地址赋值给macAddress
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    LOGI("bssid: %s, rssi: %d, connState: %d, reason: %d, ssid: %s",
        macAddress, info->rssi, info->connState, info->disconnectedReason, info->ssid);     //log打印连接信息
}

static char* SecurityTypeName(WifiSecurityType type)     //返回扫描到的wifi热点的加密方式，同样在对应的头文件中有定义
{
    switch (type)
    {
    case WIFI_SEC_TYPE_OPEN:
        return "OPEN";
    case WIFI_SEC_TYPE_WEP:
        return "WEP";
    case WIFI_SEC_TYPE_PSK:
        return "PSK";
    case WIFI_SEC_TYPE_SAE:
        return "SAE";
    default:
        break;
    }
    LOGI("unknow type = %x\r\n", type);
    return "Unknow";
}

static char* FormatMacAddress(char* buffer, unsigned char* mac)     //格式化mac地址，这里使用了snprintf（）函数，后面直接调用该函数即可，不用写下面这很长一段
{
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return buffer;
}

static void PrintScanResult(void)                              //打印热点扫描的结果 ，周围有哪些热点，及其类型，名称等         
{
    WifiScanInfo scanResult[WIFI_SCAN_HOTSPOT_LIMIT] = {0};    //默认显示最多64个结果，见头文件的定义
    uint32_t resultSize = WIFI_SCAN_HOTSPOT_LIMIT;

    memset(&scanResult, 0, sizeof(scanResult));
    WifiErrorCode errCode = GetScanInfoList(scanResult, &resultSize);
    if (errCode != WIFI_SUCCESS) {                             //打印错误信息
        LOGI("GetScanInfoList failed: %d", errCode);
    }
    for (uint32_t i = 0; i < resultSize; i++) {
        char buffer[32] = {0};
        WifiScanInfo info = scanResult[i];
        LOGI("Result[%d]: %s, %4s, %d, %d, %d, %s", i,
            FormatMacAddress(buffer, info.bssid), SecurityTypeName(info.securityType),
            info.rssi, info.band, info.frequency, info.ssid);
    }
}

static int g_connected = 0;                                             //连接状态，1表示连接成功
static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info) return;

    LOGI("%s %d, state = %d, info = ", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == WIFI_STATE_AVALIABLE) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

static int g_scanDone = 0;                                            //扫描状态，1表示扫描完毕
static void OnWifiScanStateChanged(int state, int size)
{
    LOGI("%s %d, state = %X, size = %d", __FUNCTION__, __LINE__, state, size);

    if (state == WIFI_STATE_AVALIABLE && size > 0) {
        g_scanDone = 1;
    }
}

static void hexdump(void* data, int size)           //hexdump一般用来查看"二进制"文件的十六进制编码
{
    unsigned char* ptr = data;
    printf("hexdump: %p %d: ", data, size);
    for (int i = 0; i < size; i++) {
        printf("%02X%c", ptr[i], (i+1) % 8 == 0 ? '\n' : ' ');
    }
    printf("\n");
}



static void GpioIsr(char* arg)             //GPIO中断函数，PB9中断控制wifi连接状态
{
    (void)arg;
    IoTGpioSetIsrMask(IOT_GPIO_PB_09, 0);             //不屏蔽PB9的中断功能（因为用它的中断功能确定LED的状态）
    control_flag = control_flag == 0 ? 1 : 0;         //按一次按键切换一次状态
    printf(" Wifi state changed! Now control_flag=%d\n",control_flag);  //打印当前flag值
}


static void WifiConnectTask(void *arg)          //WIFI连接任务（static静态函数，函数名只在本文件生效）
{
    (void)arg;
    WifiErrorCode errCode;        //WifiErrorCode类型结构体变量errCode，其成员是WIFI的各种状态
    WifiEvent eventListener = {   //wif事件（连接状态改变、扫描状态改变）监听，WifiEvent类型结构体变量eventListener，成员如下
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};   //连接到指定wifi AP的配置，是结构体，下面会对其成员变量赋值（ssid等）
    int netId = -1;

    osDelay(100);
    errCode = RegisterWifiEvent(&eventListener);
    LOGI("RegisterWifiEvent: %d", errCode);

    // 设置热点参数（可以自行修改蓝牙穿参、串口传参）
    strcpy(apConfig.ssid, "jaychouios");
    strcpy(apConfig.preSharedKey, "12345679");
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    while (1) 
    {
        switch (control_flag)
        {
            case 0:
            {
                LOGI("sysTicks: %u, kernelTicks: %u", osKernelGetSysTimerCount(), osKernelGetTickCount());
                LOGI("IsWifiActive: %d", IsWifiActive());  //此处wifi状态为未连接，0

                errCode = EnableWifi();                     //激活开发板的wifi，成功后EnableWifi()返回：WIFI_SUCCESS
                LOGI("EnableWifi: %d", errCode);            //激活成功此处输出0,WIFI_SUCCESS=0
                LOGI("IsWifiActive: %d", IsWifiActive());   //此处为1
                osDelay(10);

                char buffer[32] = {0};
                unsigned char mac[6] = {0};
                errCode = GetDeviceMacAddress(mac);
                LOGI("GetDeviceMacAddress: errCode:%d, FormatMacAddress:%s", errCode, FormatMacAddress(buffer, mac));

                g_scanDone = 0;                               //扫描完毕会置为1,否则while循环扫等待描
                errCode = Scan();                             //扫描wifi热点
                LOGI("Scan: %d", errCode);

                LOGI("waiting for scan done...");
                while (!g_scanDone) {
                    osDelay(5);
                }
                PrintScanResult();                            //之前定义的函数，打印扫描结果
                osDelay(100);

                errCode = AddDeviceConfig(&apConfig, &netId);  //添加wifi配置信息，返回netID=0即WIFI_SUCCESS
                LOGI("AddDeviceConfig: %d", errCode);

                g_connected = 0;
                errCode = ConnectTo(netId);                   //连接到选定的热点
                LOGI("ConnectTo netID: %d,errCode: %d", netId, errCode);

                LOGI("waiting for connect success...");
                while (!g_connected) {
                    osDelay(10);
                }
                LOGI("g_connected: %d", g_connected);
                osDelay(100);

                int i = 0;                                                  //联网业务开始
                for (struct netif* p = netif_list; p; p = p->next, i++) {   //打印ip地址，网关地址，子网掩码
                    hexdump(p->name, sizeof(p->name));
                    LOGI("[%d]: ip = %s", i, inet_ntoa(p->ip_addr));
                    LOGI("[%d]: gateway = %s", i, inet_ntoa(p->gw));
                    LOGI("[%d]: netmask = %s", i, inet_ntoa(p->netmask));
                }
                bool flag_1=true;
                while(flag_1)
                {
                    switch(control_flag)
                    {
                        case 0:break;
                        case 1:printf("\n===============1============\n\n");flag_1=false;break;
                    }  
                    osDelay(100);
                    //break;
                }

                continue; 
            }

            case 1:
            {
                errCode = Disconnect(); // disconnect with your AP
                LOGI("Disconnect: %d", errCode);

                errCode = RemoveDevice(netId); // remove AP config
                LOGI("RemoveDevice: %d", errCode);

                errCode = DisableWifi();
                LOGI("DisableWifi: %d", errCode);
                bool flag_2=1;
                while (flag_2)
                {
                    switch(control_flag)
                    {
                        case 1:break;
                        case 0:printf("\n===============0============\n\n");flag_2=false;break;
                    }  
                    osDelay(100);
                    //break;
                } 
                continue;
            }
        }     
        osDelay(100);
    }
}


static void WifiConnectDemo(void)     //开启一个线程运行wifitask
{
    osThreadAttr_t attr;

    IoTGpioInit(IOT_GPIO_PB_09);      //初始化PB9引脚
    IoTGpioSetDir(IOT_GPIO_PB_09,IOT_GPIO_DIR_IN);    // input is PB09

    IoTIoSetPull(IOT_GPIO_PB_09,IOT_GPIO_PULLHIGH);   //PB9上拉输入，上拉输入的好处就是输入的电平不会上下浮动而导致输入信号不稳定，在没有信号输入的情况下可以稳定在高电平。
    IoTGpioRegisterIsrFunc(IOT_GPIO_PB_09,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);  //中断使能，PB9,边缘触发、下降沿或低电平


    attr.name = "WifiConnectTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew(WifiConnectTask, NULL, &attr) == NULL) {
        LOGI("[WifiConnectDemo] Falied to create WifiConnectTask!");
    }
}

//SYS_RUN是标识用于初始化和启动系统运行阶段的入口
//PP_FEATURE_INIT标识用于初始化和启动应用层功能的入口
APP_FEATURE_INIT(WifiConnectDemo);
