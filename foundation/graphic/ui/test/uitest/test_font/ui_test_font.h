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

#ifndef UI_TEST_FONT_H
#define UI_TEST_FONT_H

#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include "ui_test.h"

namespace OHOS {
class UITestFont : public UITest {
public:
    UITestFont() {}
    ~UITestFont() {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;

    /**
     * @brief Test psram
     */
    void Font_FontEngine_FontConvert_Test_FontTestPsram_001();

    /**
     * @brief Test Font path
     */
    void Font_FontEngine_FontConvert_Test_FontTestFontPath_001();

    /**
     * @brief Test Set Font Id
     */
    void Font_FontEngine_FontConvert_Test_FontTestSetFontId_001();

    /**
     * @brief Test Set Font
     */
    void Font_FontEngine_FontConvert_Test_FontTestSetFont_001();

    /**
     * @brief Test Get Font Height
     */
    void Font_FontEngine_FontConvert_Test_FontTestGetFontHeight_001();

    /**
     * @brief Test get font version
     */
    void Font_FontEngine_FontConvert_Test_FontTestGetFontVersion_001();

    /**
     * @brief Test get font Id
     */
    void Font_FontEngine_FontConvert_Test_FontTestGetFontId_001();

    /**
     * @brief Test get font header
     */
    void Font_FontEngine_FontConvert_Test_FontTestGetFontHeader_001();

private:
    UIScrollView* container_ = nullptr;
    void InnerTestTitle(const char* title);
};
} // namespace OHOS
#endif // UI_TEST_FONT_H
