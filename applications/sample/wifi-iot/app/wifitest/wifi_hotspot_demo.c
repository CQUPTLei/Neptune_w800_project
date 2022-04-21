/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_hotspot.h"
#if defined(__riscv)
#include "lwip/netifapi.h"
#endif

#define LOG_TAG "wifiap"
#define LOGI(fmt, ...) printf("[%d] INFO %s " fmt "\r\n", osKernelGetTickCount(), LOG_TAG, ##__VA_ARGS__)

static volatile int g_hotspotStarted = 0;

static void OnHotspotStateChanged(int state)
{
    LOGI("OnHotspotStateChanged: %d.", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        g_hotspotStarted = 1;
    } else {
        g_hotspotStarted = 0;
    }
}

static volatile int g_joinedStations = 0;

static void PrintStationInfo(StationInfo* info)
{
    if (!info) return;
    static char buffer[32] = {0};
    unsigned char* mac = info->macAddress;
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    LOGI(" PrintStationInfo: mac=%s, reason=%d.", buffer, info->disconnectedReason);
}

static void PrintStationList()
{
    StationInfo list[WIFI_MAX_STA_NUM] = {0};
    unsigned int size = WIFI_MAX_STA_NUM;

    WifiErrorCode err = GetStationList(&list, &size);
    if (err != WIFI_SUCCESS) {
        LOGI("GetStationList failed %d", err);
    }

    for (unsigned int i = 0; i < size; i++) {
        PrintStationInfo(&list[i]);
    }
}

static void OnHotspotStaJoin(StationInfo* info)
{
    g_joinedStations++;
    PrintStationInfo(info);
    LOGI("+OnHotspotStaJoin: active stations = %d.", g_joinedStations);
    PrintStationList();
}

static void OnHotspotStaLeave(StationInfo* info)
{
    g_joinedStations--;
    PrintStationInfo(info);
    LOGI("-OnHotspotStaLeave: active stations = %d.", g_joinedStations);
}

WifiEvent g_defaultWifiEventListener = {
    .OnHotspotStaJoin = OnHotspotStaJoin,
    .OnHotspotStaLeave = OnHotspotStaLeave,
    .OnHotspotStateChanged = OnHotspotStateChanged,
};

static struct netif* g_iface = NULL;

int StartHotspot(const HotspotConfig* config)
{
    WifiErrorCode errCode = WIFI_SUCCESS;

    errCode = RegisterWifiEvent(&g_defaultWifiEventListener);
    LOGI("RegisterWifiEvent: %d", errCode);

    errCode = SetHotspotConfig(config);
    LOGI("SetHotspotConfig: %d", errCode);

    g_hotspotStarted = 0;
    errCode = EnableHotspot();
    LOGI("EnableHotspot: %d", errCode);

    while (!g_hotspotStarted) {
        osDelay(10);
    }
    LOGI("g_hotspotStarted = %d.", g_hotspotStarted);

#if defined(__riscv)
    g_iface = netifapi_netif_find("ap0");
    if (g_iface) {
        ip4_addr_t ipaddr;
        ip4_addr_t gateway;
        ip4_addr_t netmask;

        IP4_ADDR(&ipaddr,  192, 168, 1, 1);     /* input your IP for example: 192.168.1.1 */
        IP4_ADDR(&gateway, 192, 168, 1, 1);     /* input your gateway for example: 192.168.1.1 */
        IP4_ADDR(&netmask, 255, 255, 255, 0);   /* input your netmask for example: 255.255.255.0 */
        err_t ret = netifapi_netif_set_addr(g_iface, &ipaddr, &netmask, &gateway);
        LOGI("netifapi_netif_set_addr: %d", ret);

        ret = netifapi_dhcps_start(g_iface, 0, 0); // 海思扩展的HDCP服务接口
        LOGI("netifapi_dhcp_start: %d", ret);
    }
#endif
    return errCode;
}

void StopHotspot(void)
{
#if defined(__riscv)
    if (g_iface) {
        err_t ret = netifapi_dhcps_stop(g_iface);  // 海思扩展的HDCP服务接口
        LOGI("netifapi_dhcps_stop: %d", ret);
    }
#endif

    WifiErrorCode errCode = UnRegisterWifiEvent(&g_defaultWifiEventListener);
    LOGI("UnRegisterWifiEvent: %d", errCode);

    errCode = DisableHotspot();
    LOGI("EnableHotspot: %d", errCode);
}


static void WifiHotspotTask(void *arg)
{
    (void)arg;
    WifiErrorCode errCode;
    HotspotConfig config = {0};

    // 准备AP的配置参数
    strcpy(config.ssid, "HiSpark-AP");
    strcpy(config.preSharedKey, "12345678");
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = 7;

    osDelay(10);

    while (1) {
        LOGI("starting AP ...");
        LOGI("IsHotspotActive(): %d", IsHotspotActive());
        errCode = StartHotspot(&config);
        LOGI("StartHotspot: %d", errCode);
        LOGI("IsHotspotActive(): %d", IsHotspotActive());

        HotspotConfig result = {0};
        errCode = GetHotspotConfig(&result);
        LOGI("GetHotspotConfig: %d, %d", errCode, memcmp(&config, &result, sizeof(result)));

        const int step = 5;
        for (int timeout = 60; timeout >= 0; timeout -= step) {
            LOGI("after %d seconds, I will dsiable AP.", timeout);
            osDelay(step * 100);
        }

        LOGI("stop AP ...");
        StopHotspot();
        LOGI("stop AP done!");

        osDelay(100);
    }
}

static void WifiHotspotDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "WifiHotspotTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew(WifiHotspotTask, NULL, &attr) == NULL) {
        LOGI("[WifiHotspotDemo] Falied to create WifiHotspotTask!\n");
    }
}

APP_FEATURE_INIT(WifiHotspotDemo);
