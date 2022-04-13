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

#include "ui_test_font.h"
#include "common/screen.h"
#include "components/ui_checkbox.h"
#include "components/ui_label.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t GAP = 15;
const int16_t TITLE_HEIGHT = 20;
const uint16_t LABEL_WIDTH = 400;
const uint16_t LABEL_HEIGHT = 50;
const uint16_t BUF_SIZE = 200;
static uint8_t g_newFontPsramBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH];
} // namespace

void UITestFont::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - LABEL_HEIGHT);
        positionX_ = 50; // 50: init position x
        positionY_ = 5;  // 5: init position y
    }
}

void UITestFont::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

void UITestFont::InnerTestTitle(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                            TITLE_LABEL_DEFAULT_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + GAP;
}

UIView* UITestFont::GetTestView()
{
    Font_FontEngine_FontConvert_Test_FontTestSetFontId_001();
    Font_FontEngine_FontConvert_Test_FontTestSetFont_001();
    Font_FontEngine_FontConvert_Test_FontTestGetFontHeight_001();
    Font_FontEngine_FontConvert_Test_FontTestGetFontVersion_001();
    Font_FontEngine_FontConvert_Test_FontTestGetFontId_001();
    Font_FontEngine_FontConvert_Test_FontTestGetFontHeight_001();
    Font_FontEngine_FontConvert_Test_FontTestPsram_001();
    Font_FontEngine_FontConvert_Test_FontTestFontPath_001();
    return container_;
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestPsram_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Psram");
        UIFont::GetInstance()->SetPsramMemory(reinterpret_cast<uintptr_t>(g_newFontPsramBaseAddr),
                                              OHOS::MIN_FONT_PSRAM_LENGTH);
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18 : size
        char buf[BUF_SIZE] = {0};
        label->SetText(reinterpret_cast<char*>(buf));
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestFontPath_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Font Path");
        std::string dpath;
        std::string spath;
        size_t len = dpath.size();
        size_t pos = dpath.find_last_of('\\');
        dpath.replace((pos + 1), (len - pos), "..\\..\\config\\font.bin");
        spath.replace((pos + 1), (len - pos), "..\\..\\config\\glyphs.bin");
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        char buf[BUF_SIZE] = {0};
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18 : size
        label->SetText(reinterpret_cast<char*>(buf));
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestSetFontId_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Set Font Id");
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        UIFont::GetInstance()->SetCurrentFontId(0);
        label->SetFontId(0);
        label->SetText("Hello!\nOHOS UIKIT!");
        label->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestSetFont_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Set Font");
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        uint8_t fontId = UIFont::GetInstance()->GetFontId("RobotoCondensed-Regular");
        UIFont::GetInstance()->SetCurrentFontId(fontId, 18); // 18: means font size
        label->SetFont("RobotoCondensed-Regular", 18);       // 18: means font size
        label->SetText("Hello!\nOHOS UIKIT!");
        label->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestGetFontHeight_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Get Font Height");
        UILabel* label = new UILabel();
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18 : size
        char buf[BUF_SIZE] = {0};
        label->SetText(reinterpret_cast<char*>(buf));
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        positionY_ += LABEL_HEIGHT + GAP;
        container_->Add(label);
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestGetFontVersion_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Get Font Version");
        UILabel* label = new UILabel();
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        char buf[BUF_SIZE] = {0};
        label->SetText(reinterpret_cast<char*>(buf));
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        positionY_ += LABEL_HEIGHT + GAP;
        container_->Add(label);
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestGetFontId_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Get Font Id");
        UILabel* label = new UILabel();
        label->SetFont("HYQiHei-65S", 18);                   // 18: means font size
        UIFont::GetInstance()->GetFontId("HYQiHei-65S", 18); // 18: means font size
        char buf[BUF_SIZE] = {0};
        label->SetText(reinterpret_cast<char*>(buf));
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        positionY_ += LABEL_HEIGHT + GAP;
        container_->Add(label);
    }
}

void UITestFont::Font_FontEngine_FontConvert_Test_FontTestGetFontHeader_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("Test Get Font Header");
        UILabel* label = new UILabel();
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        FontHeader fontHeader;
        UIFont::GetInstance()->GetCurrentFontHeader(fontHeader);
        char buf[BUF_SIZE] = {0};
        label->SetText(reinterpret_cast<char*>(buf));
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        positionY_ += LABEL_HEIGHT + GAP;
        container_->Add(label);
    }
}
} // namespace OHOS