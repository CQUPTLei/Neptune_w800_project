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

#include <unistd.h>

#include "common/graphic_startup.h"
#include "common/screen.h"
#include "common/task_manager.h"
#include "dock/screen_device_proxy.h"
#include "font/ui_font_vector.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "window/window.h"

extern void RunApp();

namespace OHOS {
uint32_t g_animaterBuffer[HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION];
void TestAPP()
{
    WindowConfig config = {};
    config.rect.SetRect(0, 0, Screen::GetInstance().GetWidth() - 1, Screen::GetInstance().GetHeight() - 1);
    Window* window = Window::CreateWindow(config);
    if (window == nullptr) {
        GRAPHIC_LOGE("Create window false!");
        return;
    }
    window->BindRootView(RootView::GetInstance());
    RunApp();
    window->Show();
}

static void InitHal()
{
    ScreenDevice* display = new ScreenDevice();
    ScreenDeviceProxy::GetInstance()->SetDevice(display);
    ScreenDeviceProxy::GetInstance()->SetScreenSize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    ScreenDeviceProxy::GetInstance()->SetAnimatorbuffer(reinterpret_cast<uint8_t*>(g_animaterBuffer), ARGB8888,
                                                        HORIZONTAL_RESOLUTION);
}

static uint32_t g_fontPsramBaseAddr[MIN_FONT_PSRAM_LENGTH / 4];
static void InitFontEngine()
{
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontPsramBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
}
} // namespace OHOS

int main(int argc, char* argv[])
{
    OHOS::GraphicStartUp::Init();
    OHOS::InitHal();
    OHOS::InitFontEngine();
    OHOS::TestAPP();
    while (1) {
        /* Periodically call TaskHandler(). It could be done in a timer interrupt or an OS task too. */
        OHOS::TaskManager::GetInstance()->TaskHandler();
        usleep(1000 * 10); /* 1000 * 10: Just to let the system breathe */
    }
    return 0;
}
