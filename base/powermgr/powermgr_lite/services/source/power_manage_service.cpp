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

#include "power_manage_service.h"

using namespace OHOS;
using namespace OHOS::PowerMgr;

namespace {
static const int STACK_SIZE = 0x800;
static const int QUEUE_SIZE = 20;
}

PowerMgrService::PowerMgrService() : Service(), identity_()
{
    this->Service::GetName = PowerMgrService::GetServiceName;
    this->Service::Initialize = PowerMgrService::ServiceInitialize;
    this->Service::MessageHandle = PowerMgrService::ServiceMessageHandle;
    this->Service::GetTaskConfig = PowerMgrService::GetServiceTaskConfig;
}

static void GInit()
{
    SamgrLite *sm = SAMGR_GetInstance();
    if (sm == nullptr) {
        return;
    }
    sm->RegisterService(PowerMgrService::GetInstance());
}
SYSEX_SERVICE_INIT(GInit);

const char *PowerMgrService::GetServiceName(__attribute__((unused))Service *service)
{
    return POWER_MANAGE_SERVICE;
}

BOOL PowerMgrService::ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    PowerMgrService *powerService = static_cast<PowerMgrService *>(service);
    powerService->identity_ = identity;
    return TRUE;
}

BOOL PowerMgrService::ServiceMessageHandle(Service *service, Request *request)
{
    return (request != nullptr);
}

TaskConfig PowerMgrService::GetServiceTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}
