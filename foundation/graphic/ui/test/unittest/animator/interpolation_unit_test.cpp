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

#include "animator/interpolation.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr int16_t U1 = 512;
    constexpr int16_t U2 = 256;
    constexpr int16_t U3 = 128;
    constexpr int16_t U4 = 512;
}
class InterpolationTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: InterpolationGetBezierInterpolation_001
 * @tc.desc: Verify GetBezierInterpolation function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(InterpolationTest, InterpolationGetBezierInterpolation_001, TestSize.Level1)
{
    int16_t ret = Interpolation::GetBezierInterpolation(200, U1, U2, U3, U4); // 200:current change rate;
    EXPECT_EQ(ret, 379); // 379:current change rate;
    ret = Interpolation::GetBezierInterpolation(500, U1, U2, U3, U4); // 500:current change rate;
    EXPECT_EQ(ret, 273); // 273:current change rate;
    ret = Interpolation::GetBezierInterpolation(700, U1, U2, U3, U4); // 700:current change rate;
    EXPECT_EQ(ret, 289); // 289:current change rate;
    ret = Interpolation::GetBezierInterpolation(900, U1, U2, U3, U4); // 900:current change rate;
    EXPECT_EQ(ret, 394); // 394:current change rate;
}
} // namespace OHOS