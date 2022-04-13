/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_TEST_INPUT_H
#define OHOS_ACELITE_TEST_INPUT_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "component_factory.h"
#include "root_component_mock.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class InputTddTest : public testing::Test {
#else
class InputTddTest {
#endif
public:
    InputTddTest();
    ~InputTddTest(){};
    void SetUp();
    void TearDown();
    void ComponentInputAttributeTest01();
    void ComponentInputAttributeTest02();
    void ComponentInputAttributeTest06();
    void ComponentInputAttributeTest07();
    void ComponentInputAttributeTest10();
    void ComponentInputAttributeTest13();
    void ComponentInputAttributeTest14();
    void ComponentInputAttributeTest15();
    void ComponentInputStyleTest04();
    void ComponentInputStyleTest05();
    void ComponentInputStyleTest08();
    void ComponentInputStyleTest08Extra(int16_t right, int16_t top, int16_t bottom);
    void ComponentInputStyleTest09();
    void ComponentInputStyleTest11();
    void ComponentInputStyleTest12();
    void ComponentRadioAttributeTest01();
    void ComponentRadioStyleTest03();
    void ComponentRadioStyleTest04();
    UIRadioButton *ConvertView(const char *name) const;
    UIView *GetComponent() const;
    bool CompareColor(ColorType color, uint32_t colorIntValue) const;
    void AddBoolProperty(jerry_value_t object, const char *name, bool value) const;
    void AddNumberProperty(jerry_value_t object, const char *name, const double value) const;
    void AddStringProperty(jerry_value_t object, const char *name, const char *value) const;
    void RunTests();

    jerry_value_t globalObject_;
    jerry_value_t attrsObject_;
    jerry_value_t valueStaticStyle_;

private:
    RootComponentMock rootComponentMock_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_INPUT_H
