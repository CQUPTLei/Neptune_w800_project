/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "power_manage_feature.h"

using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace OHOS::HiviewDFX;

namespace {
static const int SLEEP_OUT_TIME_SEC = 300;
static const int REGISTER_WAIT_TIME_SEC = 20;
static const int POWER_ERROR = -1;
static const int PM_INVOKE_SUCCESS = 1;
}

static int32 g_lockNum = 0;
static bool g_isSleep = false;
static int g_count = 0;
static int g_targetTime = SLEEP_OUT_TIME_SEC;
static pthread_mutex_t g_numMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t g_powerThread = -1;

static void *SleepListen(void *args);
static constexpr HiLogLabel LABEL = {LOG_CORE, 0, "PowerMgrService"};

PowerManageFeature::PowerManageFeature() : Feature() {}

void PowerManageFeature::AcqurieLock()
{
    pthread_mutex_lock(&g_numMutex);
    HiLog::Debug(LABEL, "AcqurieLock.");
    g_lockNum++;
    pthread_mutex_unlock(&g_numMutex);
}

void PowerManageFeature::ReleaseLock()
{
    pthread_mutex_lock(&g_numMutex);
    HiLog::Debug(LABEL, "ReleaseLock.");
    g_lockNum--;
    g_count = 0;
    pthread_mutex_unlock(&g_numMutex);
}

int32 PowerManageFeature::IsAlwaysOn()
{
    HiLog::Debug(LABEL, "IsAlwaysOn.");
    return g_lockNum;
}

void PowerManageFeature::UserActivity::OnRawEvent(const RawEvent& event)
{
    pthread_mutex_lock(&g_numMutex);
    HiLog::Debug(LABEL, "User touch the screen, refresh time now.");
    g_count = 0;
    if (g_isSleep) {
        g_isSleep = false;
    }
    pthread_mutex_unlock(&g_numMutex);
}

PowerMgrFeatureImpl g_pmsImpl = {
    GetName : PowerManageFeature::GetFeatureName,
    OnInitialize : PowerManageFeature::OnFeatureInitialize,
    OnStop : PowerManageFeature::OnFeatureStop,
    OnMessage : PowerManageFeature::OnFeatureMessage,
    SERVER_IPROXY_IMPL_BEGIN,
    Invoke : PowerManageFeature::Invoke,
    Lock : PowerManageFeature::AcqurieLock,
    UnLock : PowerManageFeature::ReleaseLock,
    IsStayOn : PowerManageFeature::IsAlwaysOn,
    IPROXY_END,
    identity_ : { -1, -1, NULL },
};

InvokeFunc PowerManageFeature::invokeFuncList[FUNC_SIZE] {
    PowerManageFeature::LockInvoke,
    PowerManageFeature::UnLockInvoke,
    PowerManageFeature::IsAlwaysOnInvoke,
};

int32 PowerManageFeature::Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    HiLog::Debug(LABEL, "IPC Invoke called.");
    if (req == nullptr) {
        return POWER_ERROR;
    }
    if (funcId >= 0 && funcId < FUNC_SIZE) {
        if (funcId == IS_ALWAYS_ON_INVOKE) {
            IpcIoPushInt32(reply, g_lockNum);
        }
        invokeFuncList[funcId](origin, req);
    }
    return PM_INVOKE_SUCCESS;
}

int32 PowerManageFeature::LockInvoke(const void *origin, IpcIo *req)
{
    PowerManageFeature::AcqurieLock();
    return PM_INVOKE_SUCCESS;
}

int32 PowerManageFeature::UnLockInvoke(const void *origin, IpcIo *req)
{
    PowerManageFeature::ReleaseLock();
    return PM_INVOKE_SUCCESS;
}

int32 PowerManageFeature::IsAlwaysOnInvoke(const void *origin, IpcIo *req)
{
    return PowerManageFeature::IsAlwaysOn();
}

const char *PowerManageFeature::GetFeatureName(Feature *feature)
{
    return PERM_INNER;
}

void PowerManageFeature::OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    g_pmsImpl.identity_ = identity;
    HiLog::Debug(LABEL, "OnInitialize called.");
    pthread_create(&g_powerThread, nullptr, SleepListen, nullptr);
    g_lockNum = 0;
    g_isSleep = false;
    g_count = 0;
    g_targetTime = SLEEP_OUT_TIME_SEC;
}

static bool GetAmsInterface(struct AmsInterface **amsInterface)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_FEATURE);
    if (iUnknown == NULL) {
        HiLog::Debug(LABEL, "GetFeatureApi failed.");
        return false;
    }

    int32_t errCode = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)amsInterface);
    if (errCode != EC_SUCCESS) {
        HiLog::Debug(LABEL, "QueryInterface failed.");
        return false;
    }
    return true;
}

static void *SleepListen(void *args)
{
    sleep(REGISTER_WAIT_TIME_SEC);
    HiLog::Debug(LABEL, "Register ims touch event now.");
    InputEventListenerProxy::GetInstance()->RegisterInputEventListener(PowerManageFeature::UserActivity::GetInstance());
    while (true) {
        pthread_mutex_lock(&g_numMutex);
        if (g_lockNum == 0 && g_count == g_targetTime) {
            HiLog::Debug(LABEL, "Five minutes passed, It is time to call screen saver.");
            Want want = { nullptr };
            ElementName element = { nullptr };
            SetElementBundleName(&element, "com.huawei.screensaver");
            SetElementAbilityName(&element, "ScreensaverAbility");
            SetWantElement(&want, element);
            SetWantData(&want, "WantData", strlen("WantData") + 1);
            struct AmsInterface *amsInterface = nullptr;
            if (!GetAmsInterface(&amsInterface)) {
                ClearWant(&want);
            }
            amsInterface->StartAbility(&want);
            ClearElement(&element);
            ClearWant(&want);
        }
        pthread_mutex_unlock(&g_numMutex);
        g_count++;
        sleep(1);
    }
}

void PowerManageFeature::OnFeatureStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
}

BOOL PowerManageFeature::OnFeatureMessage(Feature *feature, Request *request)
{
    return ((feature != nullptr) && (request != nullptr));
}

static void GInit()
{
    HiLog::Debug(LABEL, "SYS_FEATURE_INIT called.");
    BOOL result = SAMGR_GetInstance()->RegisterFeature(POWER_MANAGE_SERVICE, (Feature *)&g_pmsImpl);
    if (result == FALSE) {
        HiLog::Debug(LABEL, "RegisterFeature failed.");
        return;
    }
    BOOL apiResult = SAMGR_GetInstance()->RegisterFeatureApi(POWER_MANAGE_SERVICE, PERM_INNER, GET_IUNKNOWN(g_pmsImpl));
    if (apiResult == FALSE) {
        HiLog::Debug(LABEL, "RegisterFeatureApi failed.");
        return;
    }
}
SYSEX_FEATURE_INIT(GInit);
