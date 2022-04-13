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

#ifndef POWER_MANAGE_SERVICE_H
#define POWER_MANAGE_SERVICE_H

#include "service.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "hilog/log.h"

static const char * const POWER_MANAGE_SERVICE = "power_service";

namespace OHOS::PowerMgr {
class PowerMgrService : public Service {
public:
    static PowerMgrService *GetInstance()
    {
        static PowerMgrService instance;
        return &instance;
    }
    PowerMgrService(const PowerMgrService&) = delete;
    PowerMgrService& operator=(const PowerMgrService&) = delete;
    PowerMgrService(PowerMgrService &&) = delete;
    PowerMgrService& operator=(PowerMgrService &&) = delete;
    ~PowerMgrService() = default;
private:
    PowerMgrService();
    static const char *GetServiceName(Service *service);
    static BOOL ServiceInitialize(Service *service, Identity identity);
    static TaskConfig GetServiceTaskConfig(Service *service);
    static BOOL ServiceMessageHandle(Service *service, Request *request);

private:
    Identity identity_;
};
} // namespace OHOS
#endif
