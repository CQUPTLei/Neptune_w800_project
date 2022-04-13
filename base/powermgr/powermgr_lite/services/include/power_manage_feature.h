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

#ifndef POWER_MANAGE_FEATURE_H
#define POWER_MANAGE_FEATURE_H

#include <pthread.h>
#include <log.h>
#include <ability_info.h>
#include <ability_slice.h>
#include <bundle_manager.h>
#include <element_name.h>
#include <module_info.h>
#include <want.h>
#include <components/ui_view.h>
#include <unistd.h>

#include "feature.h"
#include "iunknown.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "iproxy_server.h"
#include "iproxy_client.h"
#include "hilog/log.h"
#include "app_info.h"
#include "ability_context.h"
#include "ability_service_interface.h"
#include "input_event_listener_proxy.h"
#include "power_manage_service.h"

static const char * const PERM_INNER = "power_feature";

namespace OHOS::PowerMgr {
typedef int32 (*InvokeFunc)(const void *origin, IpcIo *req);

enum PowerApi {
    LOCK_INVOKE,
    UNLOCK_INVOKE,
    IS_ALWAYS_ON_INVOKE,
    FUNC_SIZE
};

class PowerManageFeature : public Feature, public InputEventListenerProxy::RawEventListener {
public:
    static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply);
    static void AcqurieLock();
    static void ReleaseLock();
    static int32 IsAlwaysOn();

    ~PowerManageFeature() = default;
    static const char *GetFeatureName(Feature *feature);
    static void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);
    static void OnFeatureStop(Feature *feature, Identity identity);
    static BOOL OnFeatureMessage(Feature *feature, Request *request);

    class UserActivity : public InputEventListenerProxy::RawEventListener {
    public:
        void OnRawEvent(const RawEvent& event)override;
        static UserActivity* GetInstance()
        {
            static UserActivity activity;
            return &activity;
        }
    };

private:
    static int32 LockInvoke(const void *origin, IpcIo *req);
    static int32 UnLockInvoke(const void *origin, IpcIo *req);
    static int32 IsAlwaysOnInvoke(const void *origin, IpcIo *req);
    static InvokeFunc invokeFuncList[FUNC_SIZE];
    PowerManageFeature();
};

struct PmsInterface {
    INHERIT_SERVER_IPROXY;
    void (*Lock)();
    void (*UnLock)();
    int32 (*IsStayOn)();
};

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(PmsInterface);
    Identity identity_;
} PowerMgrFeatureImpl;
}
#endif
