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

#include "components/ui_button.h"
#include "common/image.h"
#include "draw/draw_image.h"
#include "draw/draw_rect.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/style.h"
#include "imgdecode/cache_manager.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIButton::UIButton()
    : defaultImgSrc_(nullptr),
      triggeredImgSrc_(nullptr),
      currentImgSrc_(ButtonImageSrc::BTN_IMAGE_DEFAULT),
      imgX_(0),
      imgY_(0),
      contentWidth_(0),
      contentHeight_(0),
      state_(RELEASED),
      styleState_(RELEASED),
      buttonStyleAllocFlag_(false)
{
    touchable_ = true;
    SetupThemeStyles();
}

UIButton::~UIButton()
{
    if (defaultImgSrc_ != nullptr) {
        delete defaultImgSrc_;
        defaultImgSrc_ = nullptr;
    }

    if (triggeredImgSrc_ != nullptr) {
        delete triggeredImgSrc_;
        triggeredImgSrc_ = nullptr;
    }

    if (buttonStyleAllocFlag_) {
        for (uint8_t i = 0; i < BTN_STATE_NUM; i++) {
            delete buttonStyles_[i];
            buttonStyles_[i] = nullptr;
        }
        buttonStyleAllocFlag_ = false;
    }
}

void UIButton::DrawImg(const Rect& invalidatedArea, OpacityType opaScale)
{
    const Image* image = GetCurImageSrc();
    if (image == nullptr) {
        return;
    }

    ImageHeader header = {0};
    image->GetHeader(header);
    Rect coords;
    Rect viewRect = GetContentRect();
    coords.SetLeft(viewRect.GetLeft() + GetImageX());
    coords.SetTop(viewRect.GetTop() + GetImageY());
    coords.SetWidth(header.width);
    coords.SetHeight(header.height);

    Rect trunc(invalidatedArea);
    if (trunc.Intersect(trunc, viewRect)) {
        image->DrawImage(coords, trunc, *buttonStyles_[state_], opaScale);
    }
}

void UIButton::OnDraw(const Rect& invalidatedArea)
{
    OpacityType opa = GetMixOpaScale();
    DrawRect::Draw(GetOrigRect(), invalidatedArea, *buttonStyles_[state_], opa);
    DrawImg(invalidatedArea, opa);
}

void UIButton::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();

    if (theme == nullptr) {
        buttonStyles_[RELEASED] = &(StyleDefault::GetButtonReleasedStyle());
        buttonStyles_[PRESSED] = &(StyleDefault::GetButtonPressedStyle());
        buttonStyles_[INACTIVE] = &(StyleDefault::GetButtonInactiveStyle());
    } else {
        buttonStyles_[RELEASED] = &(theme->GetButtonStyle().released);
        buttonStyles_[PRESSED] = &(theme->GetButtonStyle().pressed);
        buttonStyles_[INACTIVE] = &(theme->GetButtonStyle().inactive);
    }
}

int64_t UIButton::GetStyle(uint8_t key) const
{
    return GetStyleForState(key, styleState_);
}

void UIButton::SetStyle(uint8_t key, int64_t value)
{
    SetStyleForState(key, value, styleState_);
}

int64_t UIButton::GetStyleForState(uint8_t key, ButtonState state) const
{
    if (state < BTN_STATE_NUM) {
        return (buttonStyles_[state])->GetStyle(key);
    }
    return 0;
}

void UIButton::SetStyleForState(uint8_t key, int64_t value, ButtonState state)
{
    if (state < BTN_STATE_NUM) {
        if (!buttonStyleAllocFlag_) {
            for (uint8_t i = 0; i < BTN_STATE_NUM; i++) {
                Style styleSaved = *buttonStyles_[i];
                buttonStyles_[i] = new Style;
                if (buttonStyles_[i] == nullptr) {
                    GRAPHIC_LOGE("new Style fail");
                    return;
                }
                *(buttonStyles_[i]) = styleSaved;
            }
            buttonStyleAllocFlag_ = true;
        }
        int16_t width = GetWidth();
        int16_t height = GetHeight();
        buttonStyles_[state]->SetStyle(key, value);
        switch (key) {
            case STYLE_BORDER_WIDTH: {
                SetWidth(width);
                SetHeight(height);
                break;
            }
            case STYLE_PADDING_LEFT:
            case STYLE_PADDING_RIGHT: {
                SetWidth(width);
                break;
            }
            case STYLE_PADDING_TOP:
            case STYLE_PADDING_BOTTOM: {
                SetHeight(height);
                break;
            }
            default:
                break;
        }
    }
}

bool UIButton::OnPressEvent(const PressEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_TRIGGERED;
    SetState(PRESSED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    return UIView::OnPressEvent(event);
}

bool UIButton::OnReleaseEvent(const ReleaseEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_DEFAULT;
    SetState(RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    return UIView::OnReleaseEvent(event);
}

bool UIButton::OnCancelEvent(const CancelEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_DEFAULT;
    SetState(RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    return UIView::OnCancelEvent(event);
}

const Image* UIButton::GetCurImageSrc() const
{
    if (currentImgSrc_ == ButtonImageSrc::BTN_IMAGE_DEFAULT) {
        return defaultImgSrc_;
    } else if (currentImgSrc_ == ButtonImageSrc::BTN_IMAGE_TRIGGERED) {
        return triggeredImgSrc_;
    } else {
        return nullptr;
    }
}

void UIButton::SetImageSrc(const char* defaultImgSrc, const char* triggeredImgSrc)
{
    if (!InitImage()) {
        return;
    }
    defaultImgSrc_->SetSrc(defaultImgSrc);
    triggeredImgSrc_->SetSrc(triggeredImgSrc);
}

void UIButton::SetImageSrc(const ImageInfo* defaultImgSrc, const ImageInfo* triggeredImgSrc)
{
    if (!InitImage()) {
        return;
    }
    defaultImgSrc_->SetSrc(defaultImgSrc);
    triggeredImgSrc_->SetSrc(triggeredImgSrc);
}

void UIButton::Disable()
{
    SetState(INACTIVE);
    touchable_ = false;
}

void UIButton::Enable()
{
    SetState(RELEASED);
    touchable_ = true;
}

void UIButton::SetState(ButtonState state)
{
    state_ = state;
    Invalidate();
}

bool UIButton::InitImage()
{
    if (defaultImgSrc_ == nullptr) {
        defaultImgSrc_ = new Image();
        if (defaultImgSrc_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    if (triggeredImgSrc_ == nullptr) {
        triggeredImgSrc_ = new Image();
        if (triggeredImgSrc_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    return true;
}

bool UIButton::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t r = buttonStyles_[styleState_]->borderRadius_;
    if (r == COORD_MAX) {
        return true;
    }
    if (r != 0) {
        r = ((r & 0x1) == 0) ? (r >> 1) : ((r + 1) >> 1);
        rect.SetLeft(rect.GetX() + r);
        rect.SetWidth(rect.GetWidth() - r);
        rect.SetTop(rect.GetY() + r);
        rect.SetHeight(rect.GetHeight() - r);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}
} // namespace OHOS