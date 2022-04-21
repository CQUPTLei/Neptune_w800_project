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
#include "wifi_device.h"

#include "lwip/netifapi.h"
// #include "lwip/api_shell.h"

#define LOG_TAG "wifista"
#define LOGI(fmt, ...) printf("[%d] INFO %s " fmt "\r\n", osKernelGetTickCount(), LOG_TAG, ##__VA_ARGS__)

static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    if (!info) return;

    static char macAddress[32] = {0};
    unsigned char* mac = info->bssid;
    snprintf(macAddress, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    LOGI("bssid: %s, rssi: %d, connState: %d, reason: %d, ssid: %s",
        macAddress, info->rssi, info->connState, info->disconnectedReason, info->ssid);
}

static char* SecurityTypeName(WifiSecurityType type)
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

static char* FormatMacAddress(char* buffer, unsigned char* mac)
{
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return buffer;
}

static void PrintScanResult(void)
{
    WifiScanInfo scanResult[WIFI_SCAN_HOTSPOT_LIMIT] = {0};
    uint32_t resultSize = WIFI_SCAN_HOTSPOT_LIMIT;

    memset(&scanResult, 0, sizeof(scanResult));
    WifiErrorCode errCode = GetScanInfoList(scanResult, &resultSize);
    if (errCode != WIFI_SUCCESS) {
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

static int g_connected = 0;
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

static int g_scanDone = 0;
static void OnWifiScanStateChanged(int state, int size)
{
    LOGI("%s %d, state = %X, size = %d", __FUNCTION__, __LINE__, state, size);

    if (state == WIFI_STATE_AVALIABLE && size > 0) {
        g_scanDone = 1;
    }
}

static void hexdump(void* data, int size)
{
    unsigned char* ptr = data;
    printf("hexdump: %p %d: ", data, size);
    for (int i = 0; i < size; i++) {
        printf("%02X%c", ptr[i], (i+1) % 8 == 0 ? '\n' : ' ');
    }
    printf("\n");
}

static void WifiConnectTask(void *arg)
{
    (void)arg;
    WifiErrorCode errCode;
    WifiEvent eventListener = {
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};
    int netId = -1;

    osDelay(100);
    errCode = RegisterWifiEvent(&eventListener);
    LOGI("RegisterWifiEvent: %d", errCode);

    // setup your AP params
    strcpy(apConfig.ssid, "HIHOPE-AP");
    strcpy(apConfig.preSharedKey, "1234567890");
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    while (1) {
        LOGI("sysTicks: %u, kernelTicks: %u", osKernelGetSysTimerCount(), osKernelGetTickCount());
        LOGI("IsWifiActive(): %d", IsWifiActive());

        errCode = EnableWifi();
        LOGI("EnableWifi: %d", errCode);
        LOGI("IsWifiActive(): %d", IsWifiActive());
        osDelay(10);

        char buffer[32] = {0};
        unsigned char mac[6] = {0};
        errCode = GetDeviceMacAddress(mac);
        LOGI("GetDeviceMacAddress: %d, %s", errCode, FormatMacAddress(buffer, mac));

        g_scanDone = 0;
        errCode = Scan();
        LOGI("Scan: %d", errCode);

        LOGI("waiting for scan done...");
        while (!g_scanDone) {
            osDelay(5);
        }
        PrintScanResult();
        osDelay(100);

        errCode = AddDeviceConfig(&apConfig, &netId);
        LOGI("AddDeviceConfig: %d", errCode);

        g_connected = 0;
        errCode = ConnectTo(netId);
        LOGI("ConnectTo(%d): %d", netId, errCode);

        LOGI("waiting for connect success...");
        while (!g_connected) {
            osDelay(10);
        }
        LOGI("g_connected: %d", g_connected);
        osDelay(100);

        // 联网业务开始
        // 这里是网络业务代码...
        int i = 0;
        for (struct netif* p = netif_list; p; p = p->next, i++) {
            hexdump(p->name, sizeof(p->name));
            LOGI("[%d]: ip = %s", i, inet_ntoa(p->ip_addr));
            LOGI("[%d]: gw = %s", i, inet_ntoa(p->gw));
            LOGI("[%d]: netmask = %s", i, inet_ntoa(p->netmask));
        }

        // 模拟联网业务
        const int step = 5;
        for (int timeout = 60; timeout >= 0; timeout -= step) {
            LOGI("after %d seconds, I will disconnect with AP.", timeout);
            osDelay(step * 100);
        }
        // 联网业务结束

        errCode = Disconnect(); // disconnect with your AP
        LOGI("Disconnect: %d", errCode);

        errCode = RemoveDevice(netId); // remove AP config
        LOGI("RemoveDevice: %d", errCode);

        errCode = DisableWifi();
        LOGI("DisableWifi: %d", errCode);
        osDelay(200);
    }
}

static void WifiConnectDemo(void)
{
    osThreadAttr_t attr;

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

APP_FEATURE_INIT(WifiConnectDemo);
