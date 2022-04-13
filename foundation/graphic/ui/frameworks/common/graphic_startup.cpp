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

#include "common/graphic_startup.h"
#include "animator/animator.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "core/render_manager.h"
#include "dfx/performance_task.h"
#include "font/ui_font.h"
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#ifdef VERSION_STANDARD
#include "dock/ohos/ohos_input_device.h"
#endif
#if ENABLE_WINDOW
#include "iwindows_manager.h"
#endif
#if ENABLE_GFX_ENGINES
#include "hals/gfx_engines.h"
#endif

namespace OHOS {
void GraphicStartUp::InitFontEngine(uintptr_t psramAddr, uint32_t psramLen, const char* dPath, const char* ttfName)
{
#if ENABLE_VECTOR_FONT
    UIFont* fontEngine = UIFont::GetInstance();
    if (fontEngine == nullptr) {
        GRAPHIC_LOGE("Get UIFont error");
        return;
    }
    fontEngine->SetPsramMemory(psramAddr, psramLen);
    // font and glyph path
    int8_t ret = fontEngine->SetFontPath(const_cast<char*>(dPath), nullptr);
    if (ret == INVALID_RET_VALUE) {
        GRAPHIC_LOGW("SetFontPath failed");
    }
    if (ttfName != nullptr) {
        uint8_t ret2 = fontEngine->RegisterFontInfo(ttfName);
        if (ret2 == INVALID_UCHAR_ID) {
            GRAPHIC_LOGW("SetTtfName failed");
        }
    }
#endif
}

void GraphicStartUp::Init()
{
    TaskManager::GetInstance()->SetTaskRun(true);
    DEBUG_PERFORMANCE_TASK_INIT();

    if (INDEV_READ_PERIOD > 0) {
        InputDeviceManager::GetInstance()->Init();
    }
    AnimatorManager::GetInstance()->Init();

    StyleDefault::Init();
    RenderManager::GetInstance().Init();

    CacheManager::GetInstance().Init(IMG_CACHE_SIZE);
#ifdef VERSION_STANDARD
    OHOSInputDevice* input = new OHOSInputDevice();
    if (input == nullptr) {
        GRAPHIC_LOGE("new OHOSInputDevice fail");
        return;
    }
    InputDeviceManager::GetInstance()->Add(input);
#endif

#if ENABLE_WINDOW
    IWindowsManager::GetInstance()->Init();
#endif
#if ENABLE_GFX_ENGINES
    GfxEngines::GetInstance()->InitDriver();
#endif
}
} // namespace OHOS
