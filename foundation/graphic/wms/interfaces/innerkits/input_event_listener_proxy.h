/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_IMS_CLIENT_PROXY_H
#define GRAPHIC_LITE_IMS_CLIENT_PROXY_H

#include <iproxy_client.h>
#include "liteipc_adapter.h"
#include "gfx_utils/input_event_info.h"

namespace OHOS {
class InputEventListenerProxy {
public:
    static InputEventListenerProxy* GetInstance()
    {
        static InputEventListenerProxy client;
        return &client;
    }

    class RawEventListener {
    public:
        RawEventListener() : alwaysInvoke_(false) {}
        virtual ~RawEventListener() {}

        void EnableAlwaysInvoke(bool alwaysInvoke)
        {
            alwaysInvoke_ = alwaysInvoke;
        }

        bool IsAlwaysInvoke()
        {
            return alwaysInvoke_;
        }
        virtual void OnRawEvent(const RawEvent& event) = 0;
    protected:
        bool alwaysInvoke_;        
    };

    bool RegisterInputEventListener(RawEventListener* listener);

    bool UnregisterInputEventListener();

private:
    InputEventListenerProxy() : proxy_(nullptr) {}
    ~InputEventListenerProxy();

    bool GetIClientProxy();
    static int32_t ReceiveMsgHandler(const IpcContext* context, void* ipcMsg, IpcIo* io, void* arg);

    IClientProxy* proxy_;
    static RawEventListener* listener_;

    InputEventListenerProxy(const InputEventListenerProxy&) = delete;
    InputEventListenerProxy& operator=(const InputEventListenerProxy&) = delete;
    InputEventListenerProxy(InputEventListenerProxy&&) = delete;
    InputEventListenerProxy& operator=(InputEventListenerProxy&&) = delete;
};
} // namespace OHOS
#endif
