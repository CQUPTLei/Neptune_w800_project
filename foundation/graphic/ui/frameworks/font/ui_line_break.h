/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_LITE_LINE_BREAK_H
#define GRAPHIC_LITE_LINE_BREAK_H

#include "graphic_config.h"
#if ENABLE_ICU
#include "font/ui_font_header.h"
#include <cstdint>
#include <string>
namespace OHOS {
/**
 * @brief line break engine.Use ICU as core.
 *
 * @since 1.0
 * @version 1.0
 */
class UILineBreakEngine : public HeapBase {
public:
    /**
     * @brief Get the Instance object
     *
     * @return UILineBreakEngine&
     * @since 1.0
     * @version 1.0
     */
    static UILineBreakEngine& GetInstance()
    {
        if (instance_ == nullptr) {
            instance_ = new UILineBreakEngine();
        }
        return *instance_;
    }

    // 0xFFFF: unlimit the length until the end null.
    uint32_t GetNextLineAndWidth(const char* txt,
                                 int16_t space,
                                 bool allBreak,
                                 int16_t &maxWidth,
                                 uint16_t len = 0xFFFF);
private:
    static UILineBreakEngine* instance_;
    UILineBreakEngine() {}
    ~UILineBreakEngine() {}
};
} // namespace OHOS
#endif // ENABLE_ICU
#endif // GRAPHIC_LITE_LINE_BREAK_H
