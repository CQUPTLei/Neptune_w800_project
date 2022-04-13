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

#include "draw/draw_utils.h"
#include "draw/draw_triangle.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_performance.h"
#include "securec.h"

#ifdef ARM_NEON_OPT
#include "graphic_neon_pipeline.h"
#include "graphic_neon_utils.h"
#endif

#if ENABLE_GFX_ENGINES
#include "hals/gfx_engines.h"
#endif

#if ENABLE_ARM_MATH
#include "arm_math.h"
#endif

namespace OHOS {
// Preprocess operation for draw
#define DRAW_UTILS_PREPROCESS(opa)                                                   \
    if ((opa) == OPA_TRANSPARENT) {                                                  \
        return;                                                                      \
    }                                                                                \
    uint8_t* screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();           \
    if (screenBuffer == nullptr) {                                                   \
        return;                                                                      \
    }                                                                                \
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();        \
    uint8_t bufferPxSize = GetByteSizeByColorMode(bufferMode);                       \
    uint16_t screenBufferWidth = ScreenDeviceProxy::GetInstance()->GetBufferWidth(); \
    Rect bufferRect = ScreenDeviceProxy::GetInstance()->GetBufferRect();

/* cover mode, src alpha is 255 */
#define COLOR_FILL_COVER(d, dm, r2, g2, b2, sm)               \
    if ((dm) == ARGB8888) {                                   \
        reinterpret_cast<Color32*>(d)->alpha = OPA_OPAQUE;    \
        if (sm == RGB565) {                                   \
            reinterpret_cast<Color32*>(d)->red = (r2) << 3;   \
            reinterpret_cast<Color32*>(d)->green = (g2) << 2; \
            reinterpret_cast<Color32*>(d)->blue = (b2) << 3;  \
        } else {                                              \
            reinterpret_cast<Color32*>(d)->red = (r2);        \
            reinterpret_cast<Color32*>(d)->green = (g2);      \
            reinterpret_cast<Color32*>(d)->blue = (b2);       \
        }                                                     \
    } else if ((dm) == RGB888) {                              \
        if (sm == RGB565) {                                   \
            reinterpret_cast<Color24*>(d)->red = (r2) << 3;   \
            reinterpret_cast<Color24*>(d)->green = (g2) << 2; \
            reinterpret_cast<Color24*>(d)->blue = (b2) << 3;  \
        } else {                                              \
            reinterpret_cast<Color24*>(d)->red = (r2);        \
            reinterpret_cast<Color24*>(d)->green = (g2);      \
            reinterpret_cast<Color24*>(d)->blue = (b2);       \
        }                                                     \
    } else if ((dm) == RGB565) {                              \
        if ((sm) == ARGB8888 || (sm) == RGB888) {             \
            reinterpret_cast<Color16*>(d)->red = (r2) >> 3;   \
            reinterpret_cast<Color16*>(d)->green = (g2) >> 2; \
            reinterpret_cast<Color16*>(d)->blue = (b2) >> 3;  \
        } else {                                              \
            reinterpret_cast<Color16*>(d)->red = (r2);        \
            reinterpret_cast<Color16*>(d)->green = (g2);      \
            reinterpret_cast<Color16*>(d)->blue = (b2);       \
        }                                                     \
    } else {                                                  \
        ASSERT(0);                                            \
    }

#define COLOR_BLEND_RGBA(r1, g1, b1, a1, r2, g2, b2, a2)  \
    const float A1 = static_cast<float>(a1) / OPA_OPAQUE; \
    const float A2 = static_cast<float>(a2) / OPA_OPAQUE; \
    const float a = 1 - (1 - A1) * (1 - A2);              \
    (r1) = (A2 * (r2) + (1 - A2) * A1 * (r1)) / a;        \
    (g1) = (A2 * (g2) + (1 - A2) * A1 * (g1)) / a;        \
    (b1) = (A2 * (b2) + (1 - A2) * A1 * (b1)) / a;        \
    (a1) = a * OPA_OPAQUE;

#define COLOR_BLEND_RGB(r1, g1, b1, r2, g2, b2, a2)                                    \
    (r1) = (((r2) * (a2)) / OPA_OPAQUE) + (((r1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (g1) = (((g2) * (a2)) / OPA_OPAQUE) + (((g1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (b1) = (((b2) * (a2)) / OPA_OPAQUE) + (((b1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE);

// 565
#define COLOR_FILL_BLEND(d, dm, s, sm, a)                                                                           \
    if ((dm) == ARGB8888) {                                                                                         \
        Color32* p = reinterpret_cast<Color32*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, sTmp->red, sTmp->green, sTmp->blue, alpha);       \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, sTmp->red, sTmp->green, sTmp->blue, a);           \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, (sTmp->red) << 3, (sTmp->green) << 2,             \
                             (sTmp->blue) << 3, a);                                                                 \
        }                                                                                                           \
    } else if ((dm) == RGB888) {                                                                                    \
        Color24* p = reinterpret_cast<Color24*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, alpha);                  \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, a);                      \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) << 3, (sTmp->green) << 2, (sTmp->blue) << 3, a); \
        }                                                                                                           \
    } else if ((dm) == RGB565) {                                                                                    \
        Color16* p = reinterpret_cast<Color16*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) >> 3, (sTmp->green) >> 2, (sTmp->blue) >> 3,     \
                            alpha);                                                                                 \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) >> 3, (sTmp->green) >> 2, (sTmp->blue) >> 3, a); \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, a);                      \
        }                                                                                                           \
    } else {                                                                                                        \
        ASSERT(0);                                                                                                  \
    }

#ifdef VERSION_STANDARD
const int16_t HARDWARE_ACC_SIZE_LIMIT = 50 * 50;
#endif

namespace {
static constexpr uint8_t OPACITY_STEP_A1 = 255;
static constexpr uint8_t OPACITY_STEP_A2 = 85;
static constexpr uint8_t OPACITY_STEP_A4 = 17;
} // namespace

TriangleEdge::TriangleEdge(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    curX = FO_TRANS_INTEGER_TO_FIXED(x1);
    curY = FO_TRANS_INTEGER_TO_FIXED(y1);
    du = FO_TRANS_INTEGER_TO_FIXED(x2 - x1);
    dv = FO_TRANS_INTEGER_TO_FIXED(y2 - y1);
}

TriangleEdge::~TriangleEdge() {}

void DrawUtils::DrawColorAreaBySides(const Rect& mask,
                                     const ColorType& color,
                                     OpacityType opa,
                                     const EdgeSides& sides) const
{
    Rect area(sides.left, sides.top, sides.right, sides.bottom);
    DrawUtils::GetInstance()->DrawColorArea(area, mask, color, opa);
}

void DrawUtils::DrawColorArea(const Rect& area, const Rect& mask, const ColorType& color, OpacityType opa) const
{
    DRAW_UTILS_PREPROCESS(opa);
    Rect maskedArea;
    if (!maskedArea.Intersect(area, mask)) {
        return;
    }
#if !ENABLE_WINDOW
    maskedArea.SetRect(maskedArea.GetLeft() - bufferRect.GetLeft(), maskedArea.GetTop() - bufferRect.GetTop(),
                       maskedArea.GetRight() - bufferRect.GetLeft(), maskedArea.GetBottom() - bufferRect.GetTop());
#endif
#if ENABLE_HARDWARE_ACCELERATION
    if (ScreenDeviceProxy::GetInstance()->HardwareFill(maskedArea, Color::ColorTo32(color), opa, screenBuffer,
        screenBufferWidth * bufferPxSize, bufferMode)) {
        return;
    }
#endif
#if ENABLE_GFX_ENGINES
    if (FillAreaWithHardware(maskedArea, color, opa)) {
        return;
    }
#endif
    FillAreaWithSoftWare(maskedArea, screenBuffer, bufferMode, bufferPxSize, screenBufferWidth, color, opa);
}

uint8_t DrawUtils::GetPxSizeByColorMode(uint8_t colorMode)
{
    switch (colorMode) {
        case ARGB8888:
            return 32; // 32: 32 bit
        case RGB888:
            return 24; // 24: 24 bit
        case RGB565:
        case ARGB1555:
        case ARGB4444:
            return 16; // 16: 16 bit
        case L1:
        case A1:
            return 1; // 1: 1 bit
        case L2:
        case A2:
            return 2; // 2: 2 bit
        case L4:
        case A4:
            return 4; // 4: 4 bit
        case L8:
        case A8:
            return 8; // 8: 8 bit
        default:
            return 0;
    }
}

uint8_t DrawUtils::GetByteSizeByColorMode(uint8_t colorMode)
{
    switch (colorMode) {
        case ARGB8888:
            return 4; // 4: 4 Byte
        case RGB888:
            return 3; // 3: 3 Byte
        case RGB565:
        case ARGB1555:
        case ARGB4444:
            return 2; // 2: 2 Byte
        default:
            return 0;
    }
}

uint8_t DrawUtils::GetPxSizeByImageInfo(ImageInfo imageInfo)
{
    if ((imageInfo.header.width == 0) || (imageInfo.header.height == 0)) {
        return 0;
    }
    /* 3 : when change byte to single pixel, the buffer should multiply by 8, equal to shift left 3 bits. */
    uint8_t pxSize = (imageInfo.dataSize / (imageInfo.header.width * imageInfo.header.height)) << 3;
    return pxSize;
}

void DrawUtils::DrawPixel(int16_t x, int16_t y, const Rect& mask, const ColorType& color, OpacityType opa) const
{
    if ((x < mask.GetLeft()) || (x > mask.GetRight()) || (y < mask.GetTop()) || (y > mask.GetBottom())) {
        return;
    }

    DRAW_UTILS_PREPROCESS(opa);

#if !ENABLE_WINDOW
    x -= bufferRect.GetLeft();
    y -= bufferRect.GetTop();
#endif

    Color32 fillColor;
    fillColor.full = Color::ColorTo32(color);

    screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
    COLOR_FILL_BLEND(screenBuffer, bufferMode, &fillColor, ARGB8888, opa);
}

void DrawUtils::DrawLetter(const LabelLetterInfo& letterInfo) const
{
    OpacityType opa = letterInfo.opa;
    Color32 fillColor;
    fillColor.full = Color::ColorTo32(letterInfo.color);

    DRAW_UTILS_PREPROCESS(opa);
    UIFont* fontEngine = UIFont::GetInstance();
    FontHeader head;
    GlyphNode node;
    if (fontEngine->GetCurrentFontHeader(head) != 0) {
        return;
    }

    const uint8_t* fontMap = fontEngine->GetBitmap(letterInfo.letter, node, letterInfo.shapingId);
    if (fontMap == nullptr) {
        return;
    }
    uint16_t letterW = node.cols;
    uint16_t letterH = node.rows;
    uint8_t opacityMask;
    int16_t posX;
    int16_t posY = letterInfo.pos.y + letterInfo.fontSize - node.top - letterInfo.offsetY;
    uint8_t fontWeight = fontEngine->GetFontWeight(letterInfo.fontId);
    uint8_t colorMode = 0;
    uint8_t opacityStep = 1;
    switch (fontWeight) {
        case FONT_WEIGHT_1:
            opacityStep = OPACITY_STEP_A1;
            opacityMask = 0x01;
            colorMode = A1;
            break;
        case FONT_WEIGHT_2:
            opacityStep = OPACITY_STEP_A2;
            opacityMask = 0x03;
            colorMode = A2;
            break;
        case FONT_WEIGHT_4:
            opacityStep = OPACITY_STEP_A4;
            opacityMask = 0x0F;
            colorMode = A4;
            break;
        case FONT_WEIGHT_8:
            opacityMask = 0xFF;
            colorMode = A8;
            break;
        default:
            return;
    }

    if (letterInfo.direct == TEXT_DIRECT_RTL) {
        /* RTL */
        posX = letterInfo.pos.x - node.advance + node.left + letterInfo.offsetX;
    } else {
        /* LTR */
        posX = letterInfo.pos.x + node.left + letterInfo.offsetX;
    }

    if ((posX + letterW < letterInfo.mask.GetLeft()) || (posX > letterInfo.mask.GetRight()) ||
        (posY + letterH < letterInfo.mask.GetTop()) || (posY > letterInfo.mask.GetBottom())) {
        return;
    }

    uint16_t rowStart = (posY >= letterInfo.mask.GetTop()) ? 0 : (letterInfo.mask.GetTop() - posY);
    uint16_t rowEnd =
        (posY + letterH <= letterInfo.mask.GetBottom()) ? letterH : (letterInfo.mask.GetBottom() - posY + 1);
    uint16_t colStart = (posX >= letterInfo.mask.GetLeft()) ? 0 : (letterInfo.mask.GetLeft() - posX);
    uint16_t colEnd =
        (posX + letterW <= letterInfo.mask.GetRight()) ? letterW : (letterInfo.mask.GetRight() - posX + 1);

    uint8_t letterWidthInByte = (letterW * fontWeight) >> SHIFT_3;
    if ((letterW * fontWeight) & 0x7) {
        letterWidthInByte++;
    }

#if ENABLE_WINDOW
    int16_t dstPosX = posX + colStart;
    int16_t dstPosY = posY + rowStart;
#else
    int16_t dstPosX = posX + colStart - bufferRect.GetLeft();
    int16_t dstPosY = posY + rowStart - bufferRect.GetTop();
#endif

#if ENABLE_HARDWARE_ACCELERATION && ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
    Rect srcRect(colStart, rowStart, colEnd - 1, rowEnd - 1);
    if (ScreenDeviceProxy::GetInstance()->HardwareBlend(fontMap, srcRect, letterWidthInByte, letterH,
        static_cast<ColorMode>(colorMode),
        Color::ColorTo32(letterInfo.color), opa, reinterpret_cast<uint8_t*>(screenBuffer),
        screenBufferWidth * bufferPxSize, bufferMode, dstPosX, dstPosY)) {
        return;
    }
#endif

    screenBuffer += ((dstPosY * screenBufferWidth) + dstPosX) * bufferPxSize;
    fontMap += (rowStart * letterWidthInByte) + ((colStart * fontWeight) >> SHIFT_3);

    uint8_t offsetInFont = (colStart * fontWeight) % FONT_WEIGHT_8;
    int16_t temp = (colEnd - colStart) * fontWeight - FONT_WEIGHT_8 + offsetInFont;
    if (temp < 0) {
        temp = 0;
    }
    int16_t validWidthInByte = temp / FONT_WEIGHT_8 + 1;
    if (temp % FONT_WEIGHT_8 != 0) {
        validWidthInByte++;
    }
    for (int16_t i = rowStart; i < rowEnd; i++) {
        int16_t col = colStart;
        uint8_t tempOffset = offsetInFont;
        uint8_t tempFontByte = (*fontMap++) >> offsetInFont;
        while (col < colEnd) {
            while ((tempOffset < FONT_WEIGHT_8) && (col < colEnd)) {
                uint8_t validOpacity = tempFontByte & opacityMask;
                if (validOpacity != 0) {
                    validOpacity *= opacityStep;
                    if (opa != OPA_OPAQUE) {
                        validOpacity =
                            static_cast<OpacityType>((static_cast<uint16_t>(validOpacity) * opa) >> FONT_WEIGHT_8);
                    }
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &fillColor, ARGB8888, validOpacity);
                }
                screenBuffer += bufferPxSize;
                tempFontByte = tempFontByte >> fontWeight;
                tempOffset += fontWeight;
                col++;
            }
            tempOffset = 0;
            tempFontByte = *(fontMap++);
        }
        fontMap += (letterWidthInByte)-validWidthInByte - 1;
        screenBuffer += (screenBufferWidth - (colEnd - colStart)) * bufferPxSize;
    }
}

void DrawUtils::DrawImage(const Rect& area,
                          const Rect& mask,
                          const uint8_t* image,
                          OpacityType opa,
                          uint8_t pxByteSize) const
{
    if (image == nullptr) {
        return;
    }
    DRAW_UTILS_PREPROCESS(opa);
    Rect maskedArea;
    Rect originMaskedArea;
    if (!originMaskedArea.Intersect(area, mask)) {
        return;
    }
#if ENABLE_WINDOW
    maskedArea = originMaskedArea;
#else
    maskedArea.SetRect(
        originMaskedArea.GetLeft() - bufferRect.GetLeft(), originMaskedArea.GetTop() - bufferRect.GetTop(),
        originMaskedArea.GetRight() - bufferRect.GetLeft(), originMaskedArea.GetBottom() - bufferRect.GetTop());
#endif
    int16_t mapWidth = area.GetWidth();
    int16_t imageX = originMaskedArea.GetLeft() - area.GetLeft();
    int16_t imageY = originMaskedArea.GetTop() - area.GetTop();

    ColorMode srcMode;
    if (pxByteSize == static_cast<uint8_t>(PixelType::IMG_RGB888)) {
        srcMode = RGB888;
    } else if (pxByteSize == static_cast<uint8_t>(PixelType::IMG_RGB565)) {
        srcMode = RGB565;
    } else if (pxByteSize == static_cast<uint8_t>(PixelType::IMG_ARGB8888)) {
        srcMode = ARGB8888;
    } else {
        GRAPHIC_LOGE("DrawUtils::DrawImage image format err\n");
        return;
    }
#if ENABLE_HARDWARE_ACCELERATION
    Rect srcRect(imageX, imageY, imageX + maskedArea.GetWidth() - 1, imageY + maskedArea.GetHeight() - 1);
    if (ScreenDeviceProxy::GetInstance()->HardwareBlend(image, srcRect, mapWidth * pxByteSize, area.GetHeight(),
        srcMode, 0, opa, screenBuffer, screenBufferWidth * bufferPxSize, bufferMode, maskedArea.GetLeft(),
        maskedArea.GetTop())) {
        return;
    }
#endif
    screenBuffer += static_cast<uint32_t>(screenBufferWidth) * maskedArea.GetTop() * bufferPxSize;
    screenBuffer += static_cast<uint32_t>(maskedArea.GetLeft()) * bufferPxSize;

    image += (static_cast<uint32_t>(mapWidth) * imageY + imageX) * pxByteSize;
    /* RGB565 RGB888 color mode, image src don't have alpha */
    BlendWithSoftWare(image, mapWidth * pxByteSize, srcMode, screenBuffer, screenBufferWidth * bufferPxSize, bufferMode,
                      maskedArea.GetWidth(), maskedArea.GetHeight(), opa);
}

#if ENABLE_GFX_ENGINES
bool DrawUtils::FillAreaWithHardware(const Rect& fillArea, const ColorType& color, const OpacityType& opa) const
{
    if ((opa != OPA_OPAQUE) && (fillArea.GetSize() >= HARDWARE_ACC_SIZE_LIMIT)) {
        AllocationInfo gfxAlloc = ScreenDeviceProxy::GetInstance()->GetAllocationInfo();
        LiteSurfaceData data;
        data.phyAddr = gfxAlloc.phyAddr;
        data.width = gfxAlloc.width;
        data.height = gfxAlloc.height;
        data.stride = gfxAlloc.stride;
        data.pixelFormat = gfxAlloc.pixelFormat;
        if (GfxEngines::GetInstance()->GfxFillArea(data, fillArea, color, opa)) {
            return true;
        }
    }
    return false;
}
#endif

void DrawUtils::FillAreaWithSoftWare(const Rect& fillArea,
                                     uint8_t* dest,
                                     ColorMode mode,
                                     uint8_t destByteSize,
                                     int16_t destWidth,
                                     const ColorType& color,
                                     const OpacityType& opa) const
{
    int32_t halBufferDeltaByteLen = static_cast<int32_t>(destWidth) * destByteSize;
    int16_t width = fillArea.GetWidth();
    int16_t height = fillArea.GetHeight();
    int32_t offset = static_cast<int32_t>(fillArea.GetTop()) * destWidth + fillArea.GetLeft();
    dest += offset * destByteSize;
    int32_t dstMaxSize = (ScreenDeviceProxy::GetInstance()->GetScreenArea() - offset) * destByteSize;
    Color32 fillColor;
    fillColor.full = Color::ColorTo32(color);
    uint8_t* dstTmp = nullptr;

    if ((fillColor.alpha == OPA_TRANSPARENT) || (opa == OPA_TRANSPARENT)) {
        return;
    }
    /* cover mode */
    if ((opa == OPA_OPAQUE) && (fillColor.alpha == OPA_OPAQUE)) {
        for (int16_t col = 0; col < width; ++col) {
            dstTmp = dest + (col * destByteSize);
            COLOR_FILL_COVER(dstTmp, mode, fillColor.red, fillColor.green, fillColor.blue, ARGB8888);
        }
        uint8_t* memStart = dest;
        int32_t memSize = static_cast<int32_t>(width) * destByteSize;
        dest += destWidth * destByteSize;
        dstMaxSize -= halBufferDeltaByteLen;
        for (int16_t row = 1; row < height; ++row) {
#ifdef ARM_NEON_OPT
            {
                DEBUG_PERFORMANCE_TRACE("memcpy_neon");
                NeonMemcpy(dest, dstMaxSize, memStart, memSize);
            }
#else
            {
                DEBUG_PERFORMANCE_TRACE("memcpy");
                if (memcpy_s(dest, dstMaxSize, memStart, memSize) != EOK) {
                    GRAPHIC_LOGE("DrawUtils::FillAreaWithSoftWare memcpy failed!\n");
                    return;
                }
            }
#endif
            dest += destWidth * destByteSize;
            dstMaxSize -= halBufferDeltaByteLen;
        }
    } else {
#ifdef ARM_NEON_OPT
        {
            DEBUG_PERFORMANCE_TRACE("FillAreaWithSoftWare_neon");
            NeonBlendPipeLine pipeLine;
            pipeLine.Construct(mode, ARGB8888, &fillColor, opa);
            int16_t step = NEON_STEP_8 * GetByteSizeByColorMode(mode);
            for (int16_t row = 0; row < height; ++row) {
                uint8_t* buf = dest;
                int16_t tmpWidth = width;
                while (tmpWidth >= NEON_STEP_8) {
                    pipeLine.Invoke(buf);
                    buf += step;
                    tmpWidth -= NEON_STEP_8;
                }
                for (int16_t i = 0; i < tmpWidth; ++i) {
                    COLOR_FILL_BLEND(buf, mode, &fillColor, ARGB8888, opa);
                    buf += destByteSize;
                }
                dest += halBufferDeltaByteLen;
            }
        }
#else
        {
            DEBUG_PERFORMANCE_TRACE("FillAreaWithSoftWare");
            for (int16_t row = 0; row < height; row++) {
                for (int16_t col = 0; col < width; col++) {
                    dstTmp = dest + (col * destByteSize);
                    COLOR_FILL_BLEND(dstTmp, mode, &fillColor, ARGB8888, opa);
                }
                dest += destWidth * destByteSize;
            }
        }
#endif
    }
}

void DrawUtils::BlendWithSoftWare(const uint8_t* src,
                                  uint32_t srcStride,
                                  ColorMode srcMode,
                                  uint8_t* dest,
                                  uint32_t destStride,
                                  ColorMode destMode,
                                  uint32_t width,
                                  uint32_t height,
                                  OpacityType opa) const
{
    uint8_t destByteSize = GetByteSizeByColorMode(destMode);
    uint8_t srcByteSize = GetByteSizeByColorMode(srcMode);
#ifdef ARM_NEON_OPT
    {
        DEBUG_PERFORMANCE_TRACE("BlendWithSoftWare_neon");
        NeonBlendPipeLine pipeLine;
        pipeLine.Construct(destMode, srcMode);
        int16_t dstStep = NEON_STEP_8 * GetByteSizeByColorMode(destMode);
        int16_t srcStep = NEON_STEP_8 * GetByteSizeByColorMode(srcMode);
        for (uint32_t row = 0; row < height; ++row) {
            uint8_t* dstBuf = dest;
            uint8_t* srcBuf = const_cast<uint8_t*>(src);
            int16_t tmpWidth = width;
            while (tmpWidth >= NEON_STEP_8) {
                pipeLine.Invoke(dstBuf, srcBuf, opa);
                dstBuf += dstStep;
                srcBuf += srcStep;
                tmpWidth -= NEON_STEP_8;
            }
            for (int16_t i = 0; i < tmpWidth; ++i) {
                COLOR_FILL_BLEND(dstBuf, destMode, srcBuf, srcMode, opa);
                dstBuf += destByteSize;
                srcBuf += srcByteSize;
            }
            dest += destStride;
            src += srcStride;
        }
    }
#else
    {
        DEBUG_PERFORMANCE_TRACE("BlendWithSoftWare");
        for (uint32_t row = 0; row < height; ++row) {
            uint8_t* destTmp = dest;
            uint8_t* srcTmp = const_cast<uint8_t*>(src);
            for (uint32_t col = 0; col < width; ++col) {
                COLOR_FILL_BLEND(destTmp, destMode, srcTmp, srcMode, opa);
                destTmp += destByteSize;
                srcTmp += srcByteSize;
            }
            dest += destStride;
            src += srcStride;
        }
    }
#endif
}

void DrawUtils::GetXAxisErrForJunctionLine(bool ignoreJunctionPoint,
                                           bool isRightPart,
                                           int16_t& xMinErr,
                                           int16_t& xMaxErr)
{
    xMinErr = 0;
    xMaxErr = 0;
    if (ignoreJunctionPoint) {
        if (isRightPart) {
            xMinErr = 1;
        } else {
            xMaxErr = -1;
        }
    }
}

void DrawUtils::GetTransformInitState(const TransformMap& transMap,
                                      const Point& position,
                                      const Rect& trans,
                                      TransformInitState& init)
{
    int16_t x = trans.GetLeft();
    int16_t y = trans.GetTop();

    init.duHorizon = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[0]);
    init.dvHorizon = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[1]);
    init.duVertical = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[3]); // 3:RSxy
    init.dvVertical = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[4]); // 4:RSyy

    init.verticalU = (x - position.x) * init.duHorizon + (y - position.y) * init.duVertical +
                     FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[6]); // 6:TRSx
    init.verticalV = (x - position.x) * init.dvHorizon + (y - position.y) * init.dvVertical +
                     FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[7]); // 7:TRSy
}

inline void DrawUtils::StepToNextLine(TriangleEdge& edge1, TriangleEdge& edge2)
{
    edge1.curY += FIXED_NUM_1;
    edge2.curY += FIXED_NUM_1;
    edge1.curX += FO_DIV(edge1.du, edge1.dv);
    edge2.curX += FO_DIV(edge2.du, edge2.dv);
}

void DrawUtils::DrawTriangleAlphaBilinear(const TriangleScanInfo& in)
{
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX);
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX);
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
#if ENABLE_WINDOW
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#else
        uint8_t* screenBuffer =
            in.screenBuffer +
            (((y - in.screenBufferRect.GetTop()) * in.screenBufferWidth + (xMin - in.screenBufferRect.GetLeft())) *
             in.bufferPxSize);
#endif
        // parameters below are Q15 fixed-point number
        int32_t u = in.init.verticalU;
        int32_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
        for (int16_t x = xMin; x <= xMax; x++) {
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
                int16_t intUPlus1 = intU + 1;
                int16_t intVPlus1 = intV + 1;
                OpacityType p1 = GetPxAlphaForAlphaImg(in.info, {intU, intV});
                OpacityType p2 = GetPxAlphaForAlphaImg(in.info, {intUPlus1, intV});
                OpacityType p3 = GetPxAlphaForAlphaImg(in.info, {intU, intVPlus1});
                OpacityType p4 = GetPxAlphaForAlphaImg(in.info, {intUPlus1, intVPlus1});
                // parameters below are Q15 fixed-point number
                int32_t decU = FO_DECIMAL(u);
                int32_t decV = FO_DECIMAL(v);
                int32_t decUMinus1 = FIXED_NUM_1 - decU;
                int32_t decVMinus1 = FIXED_NUM_1 - decV;
                int32_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int32_t w2 = FO_MUL(decU, decVMinus1);
                int32_t w3 = FO_MUL(decUMinus1, decV);
                int32_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#if ENABLE_ARM_MATH
                const int32_t outA = __SMUAD(p1, w1) + __SMUAD(p2, w2) + __SMUAD(p3, w3) + __SMUAD(p4, w4);
#else
                const int32_t outA = p1 * w1 + p2 * w2 + p3 * w3 + p4 * w4;
#endif
                Color32 result;
                result.full = Color::ColorTo32(in.color);
                result.alpha = FO_TO_INTEGER(outA);
                COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorBilinear565(const TriangleScanInfo& in)
{
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
        int16_t xMin = FO_TO_INTEGER(in.edge1.curX);
        int16_t xMax = FO_TO_INTEGER(in.edge2.curX);
#if ENABLE_WINDOW
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#else
        uint8_t* screenBuffer =
            in.screenBuffer +
            (((y - in.screenBufferRect.GetTop()) * in.screenBufferWidth + (xMin - in.screenBufferRect.GetLeft())) *
             in.bufferPxSize);
#endif
        // parameters below are Q15 fixed-point number
        int32_t u = in.init.verticalU;
        int32_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
        for (int16_t x = xMin; x <= xMax; x++) {
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                const Color16 p1 = *(reinterpret_cast<Color16*>(&imgHead[px1]));
                const Color16 p2 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.pixelSize]));
                const Color16 p3 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.srcLineWidth]));
                const Color16 p4 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));

                // parameters below are Q15 fixed-point number
                int32_t decU = FO_DECIMAL(u);
                int32_t decV = FO_DECIMAL(v);
                int32_t decUMinus1 = FIXED_NUM_1 - decU;
                int32_t decVMinus1 = FIXED_NUM_1 - decV;
                int32_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int32_t w2 = FO_MUL(decU, decVMinus1);
                int32_t w3 = FO_MUL(decUMinus1, decV);
                int32_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#if ENABLE_ARM_MATH
                const int32_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int32_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int32_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
#else
                const int32_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int32_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int32_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
#endif

                Color16 result;
                result.red = static_cast<uint8_t>(outR >>15); // 15: shift 15 bit right to convert fixed to int
                result.green = static_cast<uint8_t>(outG >> 15); // 15: shift 15 bit right to convert fixed to int
                result.blue = static_cast<uint8_t>(outB >> 15);  // 15: shift 15 bit right to convert fixed to int
                if (in.opaScale == OPA_OPAQUE) {
                    COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, RGB565);
                } else {
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, RGB565, in.opaScale);
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorBilinear888(const TriangleScanInfo& in)
{
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
        int16_t xMin = FO_TO_INTEGER(in.edge1.curX);
        int16_t xMax = FO_TO_INTEGER(in.edge2.curX);
#if ENABLE_WINDOW
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#else
        uint8_t* screenBuffer =
            in.screenBuffer +
            (((y - in.screenBufferRect.GetTop()) * in.screenBufferWidth + (xMin - in.screenBufferRect.GetLeft())) *
             in.bufferPxSize);
#endif
        // parameters below are Q15 fixed-point number
        int32_t u = in.init.verticalU;
        int32_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
        for (int16_t x = xMin; x <= xMax; x++) {
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                const Color24 p1 = *(reinterpret_cast<Color24*>(&imgHead[px1]));
                const Color24 p2 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.pixelSize]));
                const Color24 p3 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.srcLineWidth]));
                const Color24 p4 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));

                // parameters below are Q15 fixed-point number
                int32_t decU = FO_DECIMAL(u);
                int32_t decV = FO_DECIMAL(v);
                int32_t decUMinus1 = FIXED_NUM_1 - decU;
                int32_t decVMinus1 = FIXED_NUM_1 - decV;
                int32_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int32_t w2 = FO_MUL(decU, decVMinus1);
                int32_t w3 = FO_MUL(decUMinus1, decV);
                int32_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#if ENABLE_ARM_MATH
                const int32_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int32_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int32_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
#else
                const int32_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int32_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int32_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
#endif

                Color24 result;
                result.red = static_cast<uint8_t>(outR >> 15); // 15: shift 15 bit right to convert fixed to int
                result.green = static_cast<uint8_t>(outG >> 15); // 15: shift 15 bit right to convert fixed to int
                result.blue = static_cast<uint8_t>(outB >> 15);  // 15: shift 15 bit right to convert fixed to int
                if (in.opaScale == OPA_OPAQUE) {
                    COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, RGB888);
                } else {
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, RGB888, in.opaScale);
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

static void DrawTriangleTrueColorBilinear8888Inner(const TriangleScanInfo& in,
                                                   uint8_t* screenBuffer,
                                                   int16_t len,
                                                   int32_t u,
                                                   int32_t v)
{
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    for (int16_t x = 0; x < len; ++x) {
        int16_t intU = FO_TO_INTEGER(u);
        int16_t intV = FO_TO_INTEGER(v);
        if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
            uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
            uint32_t val2 = __SMUAD(intU, in.pixelSize);
            uint32_t px1 = val1 + val2;
#else
            uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
            uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
            const ColorType p1 = *(reinterpret_cast<ColorType*>(&imgHead[px1]));
            const ColorType p2 = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.pixelSize]));
            const ColorType p3 = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.srcLineWidth]));
            const ColorType p4 = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));

            // parameters below are Q15 fixed-point number
            int32_t decU = FO_DECIMAL(u);
            int32_t decV = FO_DECIMAL(v);
            int32_t decUMinus1 = FIXED_NUM_1 - decU;
            int32_t decVMinus1 = FIXED_NUM_1 - decV;
            int32_t w1 = FO_MUL(decUMinus1, decVMinus1);
            int32_t w2 = FO_MUL(decU, decVMinus1);
            int32_t w3 = FO_MUL(decUMinus1, decV);
            int32_t w4 = FO_MUL(decU, decV);
            // parameters above are Q15 fixed-point number

#if ENABLE_ARM_MATH
            const int32_t outR = __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
            const int32_t outG =
                __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
            const int32_t outB =
                __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
            const int32_t outA =
                __SMUAD(p1.alpha, w1) + __SMUAD(p2.alpha, w2) + __SMUAD(p3.alpha, w3) + __SMUAD(p4.alpha, w4);
#else
            const int32_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
            const int32_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
            const int32_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
            const int32_t outA = p1.alpha * w1 + p2.alpha * w2 + p3.alpha * w3 + p4.alpha * w4;
#endif

            Color32 result;
            result.red = static_cast<uint8_t>(outR >> 15);   // 15: shift 15 bit right to convert fixed to int
            result.green = static_cast<uint8_t>(outG >> 15); // 15: shift 15 bit right to convert fixed to int
            result.blue = static_cast<uint8_t>(outB >> 15);  // 15: shift 15 bit right to convert fixed to int
            result.alpha = static_cast<uint8_t>(outA >> 15); // 15: shift 15 bit right to convert fixed to int
            if ((in.opaScale == OPA_OPAQUE) && (result.alpha == OPA_OPAQUE)) {
                COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
            } else {
                COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
            }
        }
        u += in.init.duHorizon;
        v += in.init.dvHorizon;
        screenBuffer += in.bufferPxSize;
    }
}

#ifdef ARM_NEON_OPT
static void DrawTriangleTrueColorBilinear8888InnerNeon(const TriangleScanInfo& in,
                                                       uint8_t* screenBuffer,
                                                       int16_t len,
                                                       float u,
                                                       float v,
                                                       NeonBlendPipeLine& pipeLine)
{
    ColorType arrayp1[NEON_STEP_8] = {};
    ColorType arrayp2[NEON_STEP_8] = {};
    ColorType arrayp3[NEON_STEP_8] = {};
    ColorType arrayp4[NEON_STEP_8] = {};
    float arrayU[NEON_STEP_8] = {0};
    float arrayV[NEON_STEP_8] = {0};
    int32_t arrayPx1[NEON_STEP_8] = {0};
    int16_t step = in.bufferPxSize * NEON_STEP_8;
    float duHorizon = static_cast<float>(in.init.duHorizon) / FIXED_NUM_1;
    float dvHorizon = static_cast<float>(in.init.dvHorizon) / FIXED_NUM_1;
    while (len >= NEON_STEP_8) {
        for (uint32_t i = 0; i < NEON_STEP_8; ++i) {
            arrayU[i] = u;
            arrayV[i] = v;
            u += duHorizon;
            v += dvHorizon;
        }
        // Monotonically increasing or decreasing, so only judge the beginning and end.
        if ((arrayU[0] >= 0) && (arrayU[0] < in.info.header.width - 1) &&
            (arrayV[0] >= 0) && (arrayV[0] < in.info.header.height - 1) &&
            (arrayU[NEON_STEP_8 - 1] >= 0) && (arrayU[NEON_STEP_8 - 1] < in.info.header.width - 1) &&
            (arrayV[NEON_STEP_8 - 1] >= 0) && (arrayV[NEON_STEP_8 - 1] < in.info.header.height - 1)) {
            // Process the lower half of arrayU and arrayV
            float32x4_t vU = vld1q_f32(arrayU);
            float32x4_t vV = vld1q_f32(arrayV);
            int32x4_t vIntU = vcvtq_s32_f32(vU);
            int32x4_t vIntV = vcvtq_s32_f32(vV);
            int32x4_t vPx1 =
                vaddq_s32(vmulq_s32(vIntV, vdupq_n_s32(in.srcLineWidth)), vmulq_s32(vIntU, vdupq_n_s32(in.pixelSize)));
            vst1q_s32(arrayPx1, vPx1);
            float32x4_t vDecU = vsubq_f32(vU, vcvtq_f32_s32(vIntU));
            float32x4_t vDecV = vsubq_f32(vV, vcvtq_f32_s32(vIntV));
            float32x4_t vDecUMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecU);
            float32x4_t vDecVMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecV);
            // 256:shift 8 bit left
            uint32x4_t vLowW1 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vLowW2 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vLowW3 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecV), vdupq_n_f32(256.0)));
            uint32x4_t vLowW4 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecV), vdupq_n_f32(256.0)));
            // Process the higher half of arrayU and arrayV
            vU = vld1q_f32(arrayU + NEON_STEP_4);
            vV = vld1q_f32(arrayV + NEON_STEP_4);
            vIntU = vcvtq_s32_f32(vU);
            vIntV = vcvtq_s32_f32(vV);
            vPx1 =
                vaddq_s32(vmulq_s32(vIntV, vdupq_n_s32(in.srcLineWidth)), vmulq_s32(vIntU, vdupq_n_s32(in.pixelSize)));
            vst1q_s32(arrayPx1 + NEON_STEP_4, vPx1);
            vDecU = vsubq_f32(vU, vcvtq_f32_s32(vIntU));
            vDecV = vsubq_f32(vV, vcvtq_f32_s32(vIntV));
            vDecUMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecU);
            vDecVMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecV);
            // 256:shift 8 bit left
            uint32x4_t vHighW1 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vHighW2 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vHighW3 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecV), vdupq_n_f32(256.0)));
            uint32x4_t vHighW4 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecV), vdupq_n_f32(256.0)));

            // joins two uint32x4_t vectors into a uint16x8_t vector
            uint16x8_t vW1 = vcombine_u16(vmovn_u32(vLowW1), vmovn_u32(vHighW1));
            uint16x8_t vW2 = vcombine_u16(vmovn_u32(vLowW2), vmovn_u32(vHighW2));
            uint16x8_t vW3 = vcombine_u16(vmovn_u32(vLowW3), vmovn_u32(vHighW3));
            uint16x8_t vW4 = vcombine_u16(vmovn_u32(vLowW4), vmovn_u32(vHighW4));

            uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
            for (uint32_t i = 0; i < NEON_STEP_8; ++i) {
                int32_t px1 = arrayPx1[i];
                arrayp1[i] = *(reinterpret_cast<ColorType*>(&imgHead[px1]));
                arrayp2[i] = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.pixelSize]));
                arrayp3[i] = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.srcLineWidth]));
                arrayp4[i] = *(reinterpret_cast<ColorType*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));
            }

            uint8x8x4_t v4p1 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp1));
            uint8x8x4_t v4p2 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp2));
            uint8x8x4_t v4p3 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp3));
            uint8x8x4_t v4p4 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp4));
            uint8x8_t vOutB =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_B]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_B]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_B]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_B]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutG =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_G]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_G]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_G]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_G]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutR =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_R]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_R]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_R]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_R]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutA =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_A]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_A]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_A]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_A]), vW4),
                            8); // 8:shift 8 bit right
            vOutA = NeonMulDiv255(vdup_n_u8(in.opaScale), vOutA);
            pipeLine.Invoke(screenBuffer, vOutR, vOutG, vOutB, vOutA);
        } else {
            int32_t fixedU = FO_TRANS_FLOAT_TO_FIXED(arrayU[0]);
            int32_t fixedV = FO_TRANS_FLOAT_TO_FIXED(arrayV[0]);
            DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, NEON_STEP_8, fixedU, fixedV);
        }
        screenBuffer += step;
        len -= NEON_STEP_8;
    }
    if (len > 0) {
        int32_t fixedU = FO_TRANS_FLOAT_TO_FIXED(u);
        int32_t fixedV = FO_TRANS_FLOAT_TO_FIXED(v);
        DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, len, fixedU, fixedV);
    }
}
#endif

void DrawUtils::DrawTriangleTrueColorBilinear8888(const TriangleScanInfo& in)
{
#ifdef ARM_NEON_OPT
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
#endif
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
#ifdef ARM_NEON_OPT
    NeonBlendPipeLine pipeLine;
    pipeLine.Construct(bufferMode, ARGB8888);
#endif
    for (int16_t y = in.yMin; y <= in.yMax; ++y) {
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
#if ENABLE_WINDOW
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#else
        uint8_t* screenBuffer =
            in.screenBuffer +
            (((y - in.screenBufferRect.GetTop()) * in.screenBufferWidth + (xMin - in.screenBufferRect.GetLeft())) *
             in.bufferPxSize);
#endif
#ifdef ARM_NEON_OPT
        {
            float u = static_cast<float>(in.init.verticalU) / FIXED_NUM_1;
            float v = static_cast<float>(in.init.verticalV) / FIXED_NUM_1;
            DEBUG_PERFORMANCE_TRACE("DrawTriangleTrueColorBilinear8888_neon");
            DrawTriangleTrueColorBilinear8888InnerNeon(in, screenBuffer, xMax - xMin + 1, u, v, pipeLine);
        }
#else
        {
            int32_t u = in.init.verticalU;
            int32_t v = in.init.verticalV;
            DEBUG_PERFORMANCE_TRACE("DrawTriangleTrueColorBilinear8888");
            DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, xMax - xMin + 1, u, v);
        }
#endif
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorNearest(const TriangleScanInfo& in)
{
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
#if ENABLE_WINDOW
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#else
        uint8_t* screenBuffer =
            in.screenBuffer +
            (((y - in.screenBufferRect.GetTop()) * in.screenBufferWidth + (xMin - in.screenBufferRect.GetLeft())) *
             in.bufferPxSize);
#endif
        // parameters below are Q15 fixed-point number
        int32_t u = in.init.verticalU;
        int32_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
        for (int16_t x = xMin; x <= xMax; x++) {
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                OpacityType opa = in.opaScale;

                switch (in.info.header.colorMode) {
                    case RGB888: {
                        Color24 p24 = *(reinterpret_cast<Color24*>(&imgHead[px1]));
                        if (opa == OPA_OPAQUE) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p24.red, p24.green, p24.blue, RGB888);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p24, RGB888, opa);
                        }
                        break;
                    }
                    case RGB565: {
                        Color16 p16 = *(reinterpret_cast<Color16*>(&imgHead[px1]));
                        if (opa == OPA_OPAQUE) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p16.red, p16.green, p16.blue, RGB565);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p16, RGB565, opa);
                        }
                        break;
                    }
                    case ARGB8888: {
                        Color32 p32 = *(reinterpret_cast<Color32*>(&imgHead[px1]));
                        if ((in.opaScale == OPA_OPAQUE) && (p32.alpha == OPA_OPAQUE)) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p32.red, p32.green, p32.blue, ARGB8888);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p32, ARGB8888, in.opaScale);
                        }
                        break;
                    }
                    default:
                        return;
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTransformPart(const TrianglePartInfo& part)
{
    // parameters below are Q15 fixed-point number
    int32_t yMin = FO_TRANS_INTEGER_TO_FIXED(part.yMin);
    part.edge1.curX += (static_cast<int64_t>(part.edge1.du) *  (yMin - part.edge1.curY) / part.edge1.dv);
    part.edge1.curY = yMin;
    part.edge2.curX += (static_cast<int64_t>(part.edge2.du) *  (yMin - part.edge2.curY) / part.edge2.dv);
    part.edge2.curY = yMin;
    Rect line;
    line.SetLeft(FO_TO_INTEGER(part.edge1.curX));
    line.SetRight(FO_TO_INTEGER(part.edge1.curX));
    line.SetTop(FO_TO_INTEGER(part.edge1.curY));
    line.SetBottom(FO_TO_INTEGER(part.edge1.curY));
    // parameters above are Q15 fixed-point number
    TransformInitState init;
    GetTransformInitState(part.transMap, part.position, line, init);

    uint16_t screenBufferWidth = ScreenDeviceProxy::GetInstance()->GetBufferWidth();
    uint8_t* screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    uint8_t bufferPxSize = GetByteSizeByColorMode(bufferMode);
    if (screenBuffer == nullptr) {
        return;
    }
    Rect bufferRect = ScreenDeviceProxy::GetInstance()->GetBufferRect();
    uint8_t pixelSize;
    DrawTriangleTransformFuc fuc;
    bool isTrueColor = (part.info.header.colorMode == ARGB8888) || (part.info.header.colorMode == RGB888) ||
                       (part.info.header.colorMode == RGB565);
    if (isTrueColor) {
        pixelSize = part.info.pxSize >> SHIFT_3;
        if (part.info.algorithm == TransformAlgorithm::NEAREST_NEIGHBOR) {
            fuc = DrawTriangleTrueColorNearest;
        } else if (part.info.header.colorMode == ARGB8888) {
            fuc = DrawTriangleTrueColorBilinear8888;
        } else if (part.info.header.colorMode == RGB888) {
            fuc = DrawTriangleTrueColorBilinear888;
        } else {
            fuc = DrawTriangleTrueColorBilinear565;
        }
    } else {
        pixelSize = part.info.pxSize;
        fuc = DrawTriangleAlphaBilinear;
    }
    const int32_t srcLineWidth = part.info.header.width * pixelSize;
    TriangleScanInfo input{part.yMin,
                           part.yMax,
                           part.edge1,
                           part.edge2,
                           screenBuffer,
                           bufferPxSize,
                           bufferRect,
                           part.color,
                           part.opaScale,
                           init,
                           screenBufferWidth,
                           pixelSize,
                           srcLineWidth,
                           part.info,
                           part.mask,
                           part.isRightPart,
                           part.ignoreJunctionPoint};
    fuc(input);
}

void DrawUtils::DrawTriangleTransform(const Rect& mask,
                                      const Point& position,
                                      const ColorType& color,
                                      OpacityType opaScale,
                                      const TransformMap& transMap,
                                      const TriangleTransformDataInfo& triangleInfo)
{
    bool p3IsInRight = ((triangleInfo.p1.y - triangleInfo.p2.y) * triangleInfo.p3.x +
                        (triangleInfo.p2.x - triangleInfo.p1.x) * triangleInfo.p3.y +
                        triangleInfo.p1.x * triangleInfo.p2.y - triangleInfo.p2.x * triangleInfo.p1.y) < 0;
    TriangleEdge edge1;
    TriangleEdge edge2;
    TrianglePartInfo part{
        mask,
        transMap,
        position,
        edge1,
        edge2,
        0,
        0,
        triangleInfo.info,
        color,
        opaScale,
        triangleInfo.isRightPart,
        triangleInfo.ignoreJunctionPoint,
    };

    uint8_t yErr = 1;
    if (triangleInfo.p2.y == triangleInfo.p1.y) {
        yErr = 0;
        goto BottomHalf;
    }
    if (p3IsInRight) {
        edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p2.x, triangleInfo.p2.y);
        edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
    } else {
        edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p2.x, triangleInfo.p2.y);
        edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
    }

    part.yMin = MATH_MAX(mask.GetTop(), triangleInfo.p1.y);
    part.yMax = MATH_MIN(mask.GetBottom(), triangleInfo.p2.y);
    part.edge1 = edge1;
    part.edge2 = edge2;
    DrawTriangleTransformPart(part);
BottomHalf:
    if (triangleInfo.p2.y == triangleInfo.p3.y) {
        return;
    }

    if (triangleInfo.p2.y == triangleInfo.p1.y) {
        if (triangleInfo.p1.x < triangleInfo.p2.x) {
            edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
            edge2 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        } else {
            edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
            edge1 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        }
    } else {
        if (p3IsInRight) {
            edge1 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        } else {
            edge2 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        }
    }

    part.yMin = MATH_MAX(mask.GetTop(), triangleInfo.p2.y + yErr);
    part.yMax = MATH_MIN(mask.GetBottom(), triangleInfo.p3.y);
    part.edge1 = edge1;
    part.edge2 = edge2;
    DrawTriangleTransformPart(part);
}

void DrawUtils::DrawTransform(const Rect& mask,
                              const Point& position,
                              const ColorType& color,
                              OpacityType opaScale,
                              const TransformMap& transMap,
                              const TransformDataInfo& dataInfo) const
{
    if (opaScale == OPA_TRANSPARENT) {
        return;
    }
    Rect trans = transMap.GetBoxRect();
    trans.SetX(trans.GetX() + position.x);
    trans.SetY(trans.GetY() + position.y);
    if (!trans.Intersect(trans, mask)) {
        return;
    }
#if ENABLE_HARDWARE_ACCELERATION
    DRAW_UTILS_PREPROCESS(opaScale);
    TransformOption option;
    option.algorithm = dataInfo.algorithm;
    if (ScreenDeviceProxy::GetInstance()->HardwareTransform(dataInfo.data,
        static_cast<ColorMode>(dataInfo.header.colorMode), transMap.GetTransMapRect(), transMap.GetTransformMatrix(),
        opaScale, Color::ColorTo32(color), mask, screenBuffer, screenBufferWidth * bufferPxSize, bufferMode, option)) {
        return;
    }
#endif

    TriangleTransformDataInfo triangleInfo{
        dataInfo,
    };
    Polygon polygon = transMap.GetPolygon();
    Point p1;
    p1.x = polygon[0].x_ + position.x; // 0:first point
    p1.y = polygon[0].y_ + position.y; // 0:first point
    Point p2;
    p2.x = polygon[1].x_ + position.x; // 1:second point
    p2.y = polygon[1].y_ + position.y; // 1:second point
    Point p3;
    p3.x = polygon[2].x_ + position.x; // 2:third point
    p3.y = polygon[2].y_ + position.y; // 2:third point
    triangleInfo.isRightPart = ((p1.y - p3.y) * p2.x + (p3.x - p1.x) * p2.y + p1.x * p3.y - p3.x * p1.y) < 0;
    triangleInfo.isRightPart = (p1.y < p3.y) ? triangleInfo.isRightPart : !triangleInfo.isRightPart;
    DrawTriangle::SortVertexs(p1, p2, p3);
    triangleInfo.ignoreJunctionPoint = false;
    triangleInfo.p1 = p1;
    triangleInfo.p2 = p2;
    triangleInfo.p3 = p3;
    if ((triangleInfo.p1.y <= mask.GetBottom()) && (triangleInfo.p3.y >= mask.GetTop())) {
        DrawTriangleTransform(mask, position, color, opaScale, transMap, triangleInfo);
    }

    triangleInfo.ignoreJunctionPoint = true;
    triangleInfo.isRightPart = !triangleInfo.isRightPart;
    p1.x = polygon[0].x_ + position.x; // 0:first point
    p1.y = polygon[0].y_ + position.y; // 0:first point
    p3.x = polygon[2].x_ + position.x; // 2:third point
    p3.y = polygon[2].y_ + position.y; // 2:third point
    Point p4;
    p4.x = polygon[3].x_ + position.x; // 3:fourth point
    p4.y = polygon[3].y_ + position.y; // 3:fourth point
    DrawTriangle::SortVertexs(p1, p3, p4);
    triangleInfo.p1 = p1;
    triangleInfo.p2 = p3;
    triangleInfo.p3 = p4;
    if ((triangleInfo.p1.y <= mask.GetBottom()) && (triangleInfo.p3.y >= mask.GetTop())) {
        DrawTriangleTransform(mask, position, color, opaScale, transMap, triangleInfo);
    }
}

OpacityType DrawUtils::GetPxAlphaForAlphaImg(const TransformDataInfo& dataInfo, const Point& point)
{
    Point tmpPoint = point;
    const uint8_t* bufU8 = const_cast<uint8_t*>(dataInfo.data);
#if ENABLE_SPEC_FONT
    if (dataInfo.header.colorMode == A1) {
        uint8_t bit = tmpPoint.x & 0x7; // 0x7: 1 byte is 8 bit,
        tmpPoint.x = tmpPoint.x >> SHIFT_3;

        uint32_t px = (dataInfo.header.width >> SHIFT_3) * tmpPoint.y + tmpPoint.x;
        // 1: A1 means 1 bit, 7: maximum offset in bytes
        uint8_t pxOpa = (bufU8[px] & (1 << (7 - bit))) >> (7 - bit);
        return pxOpa ? OPA_TRANSPARENT : OPA_OPAQUE;
    } else if (dataInfo.header.colorMode == A2) {
        uint8_t bit = (tmpPoint.x & 0x3) * 2; // 0x3: 0b0011, 2: A2 color mode
        tmpPoint.x = tmpPoint.x >> SHIFT_2;

        uint32_t px = (dataInfo.header.width >> SHIFT_2) * tmpPoint.y + tmpPoint.x;
        // 3: the value of 0b0011
        uint8_t pxOpa = (bufU8[px] & (3 << (SHIFT_6 - bit))) >> (SHIFT_6 - bit);
        return pxOpa * OPACITY_STEP_A2;
    } else if (dataInfo.header.colorMode == A8) {
        uint32_t px = dataInfo.header.width * tmpPoint.y + tmpPoint.x;
        return bufU8[px];
    }
#else
    uint8_t letterWidthInByte = (dataInfo.header.width * dataInfo.pxSize) >> SHIFT_3;
    // 0x7: for rounding
    if ((dataInfo.header.width * dataInfo.pxSize) & 0x7) {
        letterWidthInByte++;
    }
    uint8_t bit = (tmpPoint.x & 0x1) << SHIFT_2;
    bufU8 += (tmpPoint.y * letterWidthInByte) + ((tmpPoint.x * dataInfo.pxSize) >> SHIFT_3);
    // 0xF: 0b1111, get the data of the A4 color mode
    uint8_t pxOpa = (*bufU8 & (0xF << bit)) >> (bit);
    return pxOpa * OPACITY_STEP_A4;
#endif // ENABLE_SPEC_FONT
}

void DrawUtils::DrawTranspantArea(const Rect& rect, const Rect& mask)
{
    FillArea(rect, mask, true, nullptr);
}

void DrawUtils::DrawWithBuffer(const Rect& rect, const Rect& mask, const ColorType* colorBuf)
{
    FillArea(rect, mask, false, colorBuf);
}

void DrawUtils::FillArea(const Rect& rect, const Rect& mask, bool isTransparent, const ColorType* colorBuf)
{
    Rect maskedArea;
    if (!maskedArea.Intersect(rect, mask)) {
        return;
    }

    int16_t left = maskedArea.GetLeft();
    int16_t right = maskedArea.GetRight();
    int16_t top = maskedArea.GetTop();
    int16_t bottom = maskedArea.GetBottom();

#if !ENABLE_WINDOW
    Rect bufferRect = ScreenDeviceProxy::GetInstance()->GetBufferRect();
    maskedArea.SetLeft(left - bufferRect.GetLeft());
    maskedArea.SetRight(right - bufferRect.GetLeft());
    maskedArea.SetTop(top - bufferRect.GetTop());
    maskedArea.SetBottom(bottom - bufferRect.GetTop());
#endif

    uint16_t screenBufferWidth = ScreenDeviceProxy::GetInstance()->GetBufferWidth();
    uint8_t* mem = ScreenDeviceProxy::GetInstance()->GetBuffer();
    ColorMode bufferMode = ScreenDeviceProxy::GetInstance()->GetBufferMode();
    uint8_t bufferPxSize = GetByteSizeByColorMode(bufferMode);

    mem += top * screenBufferWidth * bufferPxSize;
    if (isTransparent) {
        uint16_t sz = (right - left + 1) * bufferPxSize;
        for (int16_t row = top; row <= bottom; row++) {
            if (memset_s(mem + (left * bufferPxSize), sz, 0, sz) != EOK) {
                return;
            }
            mem += screenBufferWidth * bufferPxSize;
        }
    } else {
        if (colorBuf == nullptr) {
            return;
        }
        for (int16_t row = top; row <= bottom; row++) {
            for (int16_t col = left; col <= right; col++) {
#if COLOR_DEPTH == 32
                COLOR_FILL_COVER(mem[col * bufferPxSize], bufferMode, colorBuf[row * screenBufferWidth + col].red,
                                 colorBuf[row * screenBufferWidth + col].green,
                                 colorBuf[row * screenBufferWidth + col].blue, ARGB8888);
#else
                COLOR_FILL_COVER(mem[col * bufferPxSize], bufferMode, colorBuf[row * screenBufferWidth + col].red,
                                 colorBuf[row * screenBufferWidth + col].green,
                                 colorBuf[row * screenBufferWidth + col].blue, RGB565);
#endif
            }
            mem += screenBufferWidth * bufferPxSize;
        }
    }
}

void DrawUtils::DrawAdjPixelInLine(int16_t x1,
                                   int16_t y1,
                                   int16_t x2,
                                   int16_t y2,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(opa);
    Color32 result;
    result.full = Color::ColorTo32(color);
    if ((x1 >= mask.GetLeft()) && (x1 <= mask.GetRight()) && (y1 >= mask.GetTop()) && (y1 <= mask.GetBottom())) {
        x1 -= bufferRect.GetLeft();
        y1 -= bufferRect.GetTop();
        screenBuffer += (y1 * screenBufferWidth + x1) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((x2 >= mask.GetLeft()) && (x2 <= mask.GetRight()) && (y2 >= mask.GetTop()) && (y2 <= mask.GetBottom())) {
        x2 -= bufferRect.GetLeft();
        y2 -= bufferRect.GetTop();
        screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
        screenBuffer += (y2 * screenBufferWidth + x2) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawPixelInLine(int16_t x,
                                int16_t y,
                                const Rect& mask,
                                const ColorType& color,
                                OpacityType opa,
                                uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(opa);
    Color32 result;
    result.full = Color::ColorTo32(color);
    if ((x >= mask.GetLeft()) && (x <= mask.GetRight()) && (y >= mask.GetTop()) && (y <= mask.GetBottom())) {
        x -= bufferRect.GetLeft();
        y -= bufferRect.GetTop();
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawVerPixelInLine(int16_t x,
                                   int16_t y,
                                   int8_t dir,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(opa);
    if ((y < mask.GetTop()) || (y > mask.GetBottom())) {
        return;
    }
    Color32 result;
    result.full = Color::ColorTo32(color);
    int16_t x0 = x + dir;
    int16_t x1 = x - dir;
    y -= bufferRect.GetTop();
    if ((x0 >= mask.GetLeft()) && (x0 <= mask.GetRight())) {
        x0 -= bufferRect.GetLeft();
        screenBuffer += (y * screenBufferWidth + x0) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((x >= mask.GetLeft()) && (x <= mask.GetRight())) {
        x -= bufferRect.GetLeft();
        screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        if (opa == OPA_OPAQUE) {
            COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
        } else {
            COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, opa);
        }
    }
    if ((x1 >= mask.GetLeft()) && (x1 <= mask.GetRight())) {
        x1 -= bufferRect.GetLeft();
        screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
        screenBuffer += (y * screenBufferWidth + x1) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawHorPixelInLine(int16_t x,
                                   int16_t y,
                                   int8_t dir,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(opa);
    if ((x < mask.GetLeft()) || (x > mask.GetRight())) {
        return;
    }
    Color32 result;
    result.full = Color::ColorTo32(color);
    int16_t y0 = y + dir;
    int16_t y1 = y - dir;
    x -= bufferRect.GetLeft();
    if ((y0 >= mask.GetTop()) && (y0 <= mask.GetBottom())) {
        y0 -= bufferRect.GetTop();
        screenBuffer += (y0 * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((y >= mask.GetTop()) && (y <= mask.GetBottom())) {
        y -= bufferRect.GetTop();
        screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        if (opa == OPA_OPAQUE) {
            COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
        } else {
            COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, opa);
        }
    }
    if ((y1 >= mask.GetTop()) && (y1 <= mask.GetBottom())) {
        y1 -= bufferRect.GetTop();
        screenBuffer = ScreenDeviceProxy::GetInstance()->GetBuffer();
        screenBuffer += (y1 * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}
} // namespace OHOS