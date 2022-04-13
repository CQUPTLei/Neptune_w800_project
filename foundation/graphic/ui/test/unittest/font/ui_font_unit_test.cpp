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

#include "font/ui_font.h"
#include "font/ui_font_vector.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr uint8_t FONT_ERROR_RET = 0xFF;
}
class UIFontTest : public testing::Test {
public:
    UIFontTest() {}
    virtual ~UIFontTest() {}

    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
};

/**
 * @tc.name: Graphic_Font_Test_GetInstance_001
 * @tc.desc: Verify UIFont::GetInstance function, not nullptr.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEK
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetInstance_001, TestSize.Level0)
{
    UIFont* font = UIFont::GetInstance();
    EXPECT_NE(font, nullptr);
}

/**
 * @tc.name: Graphic_Font_Test_GetInstance_002
 * @tc.desc: Verify UIFont::GetInstance function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEK
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetInstance_002, TestSize.Level0)
{
    UIFont* font = UIFont::GetInstance();
    bool ret = UIFont::GetInstance()->IsVectorFont();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: Graphic_Font_Test_RegisterFontInfo_001
 * @tc.desc: Verify UIFont::RegisterFontInfo function, error font name.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_RegisterFontInfo_001, TestSize.Level0)
{
    uint8_t ret = UIFont::GetInstance()->RegisterFontInfo("error");
    EXPECT_EQ(ret, FONT_ERROR_RET);
}

/**
 * @tc.name: Graphic_Font_Test_RegisterFontInfo_002
 * @tc.desc: Verify UIFont::RegisterFontInfo function, error font file path.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_RegisterFontInfo_002, TestSize.Level0)
{
    uint8_t ret = UIFont::GetInstance()->RegisterFontInfo("ui-font.ttf");
    EXPECT_EQ(ret, FONT_ERROR_RET);
}

/**
 * @tc.name: Graphic_Font_Test_UnregisterFontInfo_001
 * @tc.desc: Verify UIFont::UnregisterFontInfo function, error font name.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_UnregisterFontInfo_001, TestSize.Level0)
{
    uint8_t ret = UIFont::GetInstance()->UnregisterFontInfo("error font name");
    EXPECT_EQ(ret, FONT_ERROR_RET);
}

/**
 * @tc.name: Graphic_Font_Test_UnregisterFontInfo_002
 * @tc.desc: Verify UIFont::UnregisterFontInfo function, unregister fontsTable.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_UnregisterFontInfo_002, TestSize.Level0)
{
    const UITextLanguageFontParam* fontsTable = nullptr;
    uint8_t ret = UIFont::GetInstance()->UnregisterFontInfo(fontsTable, 0);
    EXPECT_EQ(ret, 0);
}
} // namespace OHOS