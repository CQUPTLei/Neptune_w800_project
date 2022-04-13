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

#include "dock/screen_device_proxy.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
#if ENABLE_FRAME_BUFFER
void ScreenDeviceProxy::Flush() {}
#else
void ScreenDeviceProxy::Flush()
{
    flush_.Flushing();

    if (device_ != nullptr) {
#if ENABLE_WINDOW
        device_->Flush(bufferRect_.GetLeft(), bufferRect_.GetTop(), bufferRect_.GetRight(), bufferRect_.GetBottom(),
                       gfxAlloc_.virAddr, ARGB8888);
#else
        device_->Flush(bufferRect_.GetLeft(), bufferRect_.GetTop(), bufferRect_.GetRight(), bufferRect_.GetBottom(),
                       buffer_, ARGB8888);
#endif
    }
}
#endif

void ScreenDeviceProxy::OnFlushReady()
{
    flush_.Notify();
}

void ScreenDeviceProxy::OnRenderFinish()
{
    if (device_ != nullptr) {
        device_->RenderFinish();
    }
}

void ScreenDeviceProxy::DrawAnimatorBuffer(const Rect& invalidatedArea)
{
    Rect invalidRect = curViewRect_;
    transMap_.SetTransMapRect(curViewRect_);
    invalidRect.Join(invalidRect, transMap_.GetBoxRect());

    if (invalidRect.Intersect(invalidRect, invalidatedArea)) {
        uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(animatorImageInfo_.header.colorMode);
        TransformDataInfo imageTranDataInfo = {animatorImageInfo_.header, animatorImageInfo_.data, pxSize, LEVEL0,
                                               BILINEAR};
        DrawUtils::GetInstance()->DrawTransform(invalidRect, {0, 0}, Color::Black(), OPA_OPAQUE, transMap_,
                                                imageTranDataInfo);
    }
}

void ScreenDeviceProxy::SetAnimatorRect(const Rect& rect)
{
    curViewRect_ = rect;
    uint16_t bufferWidth = (width_ > curViewRect_.GetWidth()) ? curViewRect_.GetWidth() : width_;
    uint16_t bufferHeight = (height_ > curViewRect_.GetHeight()) ? curViewRect_.GetHeight() : height_;

    animatorImageInfo_.header.colorMode = animatorBufferMode_;
    animatorImageInfo_.dataSize = bufferWidth * bufferHeight * DrawUtils::GetByteSizeByColorMode(animatorBufferMode_);
    animatorImageInfo_.header.width = bufferWidth;
    animatorImageInfo_.header.height = bufferHeight;
    animatorImageInfo_.header.reserved = 0;
    animatorImageInfo_.data = reinterpret_cast<uint8_t*>(GetBuffer());
    if (animatorImageInfo_.data == nullptr) {
        return;
    }

    SetAnimatorbufferWidth(bufferWidth);
    if (memset_s(reinterpret_cast<void*>(const_cast<uint8_t*>(animatorImageInfo_.data)), animatorImageInfo_.dataSize, 0,
                 animatorImageInfo_.dataSize) != EOK) {
        GRAPHIC_LOGE("animator buffer memset failed.");
    }
}

void ScreenDeviceProxy::SetScreenSize(uint16_t width, uint16_t height)
{
    if ((width == 0) || (height == 0)) {
        GRAPHIC_LOGE("screen size can not be zero.");
        return;
    }
    width_ = width;
    height_ = height;
#if !ENABLE_WINDOW && !ENABLE_FRAME_BUFFER
    if (buffer_ != nullptr) {
        UIFree(buffer_);
    }
    uint32_t bufSize = width * height * DrawUtils::GetByteSizeByColorMode(ARGB8888);
    buffer_ = static_cast<uint8_t*>(UIMalloc(bufSize));
    if (buffer_ == nullptr) {
        GRAPHIC_LOGE("screen buffer malloc failed.");
        return;
    }
    if (memset_s(buffer_, bufSize, 0, bufSize) != EOK) {
        GRAPHIC_LOGE("screen buffer memset failed.");
        UIFree(reinterpret_cast<void*>(buffer_));
        buffer_ = nullptr;
    }
#endif
}

uint8_t* ScreenDeviceProxy::GetBuffer()
{
    flush_.Wait();
    if (useAnimatorBuff_) {
        if (animatorBufferAddr_ == nullptr) {
            GRAPHIC_LOGE("Invalid param animatorBufferAddr_.");
            return nullptr;
        }
        int32_t offset = bufferRect_.GetTop() * animatorBufferWidth_ + bufferRect_.GetLeft();
        offset *= DrawUtils::GetByteSizeByColorMode(animatorBufferMode_);
        return animatorBufferAddr_ + offset;
    }
#if ENABLE_FRAME_BUFFER
    if (frameBufferAddr_ == nullptr) {
        GRAPHIC_LOGE("Invalid param frameBufferAddr_.");
        return nullptr;
    }
    int32_t offset = bufferRect_.GetTop() * frameBufferWidth_ + bufferRect_.GetLeft();
    offset *= DrawUtils::GetByteSizeByColorMode(frameBufferMode_);
    return frameBufferAddr_ + offset;
#elif ENABLE_WINDOW
    return gfxAlloc_.virAddr;
#else
    return buffer_;
#endif
}

ColorMode ScreenDeviceProxy::GetBufferMode()
{
    if (useAnimatorBuff_) {
        return animatorBufferMode_;
    }
#if ENABLE_FRAME_BUFFER
    return frameBufferMode_;
#else
    return ARGB8888;
#endif
}
} // namespace OHOS
