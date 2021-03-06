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

#include "components/ui_abstract_progress.h"
#include "common/image.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIAbstractProgress::UIAbstractProgress()
    : enableBackground_(true),
      backgroundStyleAllocFlag_(false),
      foregroundStyleAllocFlag_(false),
      backgroundImage_(nullptr),
      foregroundImage_(nullptr),
      rangeMax_(MAX_PERCENT_VALUE),
      rangeMin_(MIN_PERCENT_VALUE),
      curValue_(0),
      step_(1),
      lastValue_(0)
{
    style_ = &(StyleDefault::GetBackgroundTransparentStyle());
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        backgroundStyle_ = &(theme->GetProgressBackgroundStyle());
        foregroundStyle_ = &(theme->GetProgressForegroundStyle());
    } else {
        backgroundStyle_ = &(StyleDefault::GetProgressBackgroundStyle());
        foregroundStyle_ = &(StyleDefault::GetProgressForegroundStyle());
    }
}

UIAbstractProgress::~UIAbstractProgress()
{
    if (backgroundImage_ != nullptr) {
        delete backgroundImage_;
        backgroundImage_ = nullptr;
    }

    if (foregroundImage_ != nullptr) {
        delete foregroundImage_;
        foregroundImage_ = nullptr;
    }

    if (backgroundStyleAllocFlag_) {
        delete backgroundStyle_;
        backgroundStyle_ = nullptr;
        backgroundStyleAllocFlag_ = false;
    }

    if (foregroundStyleAllocFlag_) {
        delete foregroundStyle_;
        foregroundStyle_ = nullptr;
        foregroundStyleAllocFlag_ = false;
    }
}

void UIAbstractProgress::SetRange(int32_t rangeMax, int32_t rangeMin)
{
    if ((rangeMax_ == rangeMax) && (rangeMin_ == rangeMin)) {
        return;
    }

    if (rangeMax >= rangeMin) {
        rangeMax_ = rangeMax;
        rangeMin_ = rangeMin;
        lastValue_ = rangeMin;
        SetValue(curValue_);
    } else {
        GRAPHIC_LOGW("UIAbstractProgress::SetRange rangeMax less than rangeMin !\n");
    }
};

void UIAbstractProgress::SetValue(int32_t value)
{
    if (value < rangeMin_) {
        curValue_ = rangeMin_;
    } else if (value > rangeMax_) {
        curValue_ = rangeMax_;
    } else {
        curValue_ = value;
    }

    if ((curValue_ != lastValue_) &&
        ((curValue_ == rangeMin_) || (curValue_ == rangeMax_) ||
        (MATH_ABS(curValue_ - lastValue_) >= static_cast<int32_t>(step_)))) {
        Invalidate();
        lastValue_ = curValue_;
    }
}

int16_t UIAbstractProgress::GetCurrentPos(int16_t distance) const
{
    uint32_t delta = lastValue_ - rangeMin_;
    uint32_t rangeSize = GetRangeSize();
    if (rangeSize == 0) {
        return distance;
    }
    int16_t result = static_cast<int64_t>(distance) * delta / rangeSize;
    return result;
}

uint32_t UIAbstractProgress::GetRangeSize() const
{
    return (rangeMax_ < rangeMin_) ? 0 : (rangeMax_ - rangeMin_);
}

void UIAbstractProgress::SetImage(const char* foregroundImage, const char* backgroundImage)
{
    if (!InitImage()) {
        return;
    }
    backgroundImage_->SetSrc(backgroundImage);
    foregroundImage_->SetSrc(foregroundImage);
}

void UIAbstractProgress::SetImage(const ImageInfo* foregroundImage, const ImageInfo* backgroundImage)
{
    if (!InitImage()) {
        return;
    }
    backgroundImage_->SetSrc(backgroundImage);
    foregroundImage_->SetSrc(foregroundImage);
}

void UIAbstractProgress::SetBackgroundStyle(const Style& style)
{
    if (!backgroundStyleAllocFlag_) {
        backgroundStyle_ = new Style();
        if (backgroundStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style1 fail");
            return;
        }
        backgroundStyleAllocFlag_ = true;
    }
    *backgroundStyle_ = style;
}

void UIAbstractProgress::SetBackgroundStyle(uint8_t key, int64_t value)
{
    if (!backgroundStyleAllocFlag_) {
        backgroundStyle_ = new Style(*backgroundStyle_);
        if (backgroundStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style1 fail");
            return;
        }
        backgroundStyleAllocFlag_ = true;
    }
    backgroundStyle_->SetStyle(key, value);
}

const Style& UIAbstractProgress::GetBackgroundStyle() const
{
    return *backgroundStyle_;
}

int64_t UIAbstractProgress::GetBackgroundStyle(uint8_t key) const
{
    return backgroundStyle_->GetStyle(key);
}

void UIAbstractProgress::SetForegroundStyle(const Style& style)
{
    if (!foregroundStyleAllocFlag_) {
        foregroundStyle_ = new Style();
        if (foregroundStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style1 fail");
            return;
        }
        foregroundStyleAllocFlag_ = true;
    }
    *foregroundStyle_ = style;
}

void UIAbstractProgress::SetForegroundStyle(uint8_t key, int64_t value)
{
    if (!foregroundStyleAllocFlag_) {
        foregroundStyle_ = new Style(*foregroundStyle_);
        if (foregroundStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style1 fail");
            return;
        }
        foregroundStyleAllocFlag_ = true;
    }
    foregroundStyle_->SetStyle(key, value);
}

const Style& UIAbstractProgress::GetForegroundStyle() const
{
    return *foregroundStyle_;
}

int64_t UIAbstractProgress::GetForegroundStyle(uint8_t key) const
{
    return foregroundStyle_->GetStyle(key);
}

bool UIAbstractProgress::InitImage()
{
    if (backgroundImage_ == nullptr) {
        backgroundImage_ = new Image();
        if (backgroundImage_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    if (foregroundImage_ == nullptr) {
        foregroundImage_ = new Image();
        if (foregroundImage_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    return true;
}
} // namespace OHOS
