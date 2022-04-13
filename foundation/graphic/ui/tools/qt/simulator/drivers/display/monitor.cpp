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

#include "monitor.h"
#include "common/graphic_startup.h"
#include "common/image_decode_ability.h"
#include "common/input_device_manager.h"
#include "dock/screen_device_proxy.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#include "font/ui_font_vector.h"
#include "mousewheel_input.h"
#include "mouse_input.h"

namespace OHOS {
void Monitor::Flush(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const uint8_t* buffer, ColorMode mode)
{
    if (buffer == nullptr) {
        ScreenDeviceProxy::GetInstance()->OnFlushReady();
        return;
    }

    if ((x1 < 0) || (y1 < 0) || (x2 > HORIZONTAL_RESOLUTION - 1) || (y2 > VERTICAL_RESOLUTION - 1)) {
        ScreenDeviceProxy::GetInstance()->OnFlushReady();
        return;
    }

    int32_t x, y;
    if (mode == ARGB8888) {
        const Color32* tmp = reinterpret_cast<const Color32*>(buffer);
        for (y = y1; y <= y2; y++) {
            for (x = x1; x <= x2; x++) {
                tftFb_[y * HORIZONTAL_RESOLUTION + x] = tmp->full;
                tmp++;
            }
        }
    }
    ScreenDeviceProxy::GetInstance()->OnFlushReady();
}

void Monitor::InitHal()
{
    ScreenDeviceProxy::GetInstance()->SetScreenSize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
#if ENABLE_FRAME_BUFFER
    ScreenDeviceProxy::GetInstance()->SetFramebuffer(reinterpret_cast<uint8_t*>(tftFb_), ARGB8888,
                                                     HORIZONTAL_RESOLUTION);
#endif
    ScreenDeviceProxy::GetInstance()->SetAnimatorbuffer(reinterpret_cast<uint8_t*>(animaterBuffer_), ARGB8888,
                                                        HORIZONTAL_RESOLUTION);
    Monitor* display = Monitor::GetInstance();
    ScreenDeviceProxy::GetInstance()->SetDevice(display);

#if USE_MOUSE
    MouseInput* mouse = MouseInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(mouse);
#endif

#if USE_MOUSEWHEEL && ENABLE_ROTATE_INPUT
    MousewheelInput* mousewheel = MousewheelInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(mousewheel);
#endif
}

void Monitor::RenderFinish()
{
    UpdatePaint(tftFb_, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

// assuming below are the memory pool
static uint8_t g_fontPsramBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH];

void Monitor::InitFontEngine()
{
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontPsramBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
}

void Monitor::InitImageDecodeAbility()
{
    uint32_t imageType = IMG_SUPPORT_BITMAP | OHOS::IMG_SUPPORT_JPEG | OHOS::IMG_SUPPORT_PNG;
    ImageDecodeAbility::GetInstance().SetImageDecodeAbility(imageType);
}

void Monitor::GUILoopStart() const
{
    Sleep(GUI_REFR_PERIOD);
}

void Monitor::InitGUI()
{
    for (uint32_t i = 0; i < HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION; i++) {
        tftFb_[i] = defaultColor_;
    }
    UpdatePaint(tftFb_, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

void Monitor::GUILoopQuit() const {}
} // namespace OHOS
