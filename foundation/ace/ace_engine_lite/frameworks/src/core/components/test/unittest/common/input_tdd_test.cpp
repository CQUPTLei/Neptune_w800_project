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

#include "input_tdd_test.h"
#include "ace_log.h"
#include "component_factory.h"
#include "input_button_component.h"
#include "input_checkbox_component.h"
#include "input_radio_component.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_fwk_common.h"
#include "style.h"
#include "ui_label_button.h"
#include "ui_view.h"

namespace OHOS {
namespace ACELite {
InputTddTest::InputTddTest()
    : globalObject_(UNDEFINED),
      attrsObject_(UNDEFINED),
      valueStaticStyle_(UNDEFINED) {}

void InputTddTest::SetUp()
{
    JsAppEnvironment *environment = JsAppEnvironment::GetInstance();
    environment->InitJsFramework();
    globalObject_ = jerry_get_global_object();
    const char *attrName = "attrs";
    jerry_value_t keyAttrs = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attrName));
    attrsObject_ = jerry_create_object();
    jerry_release_value(jerry_set_property(globalObject_, keyAttrs, attrsObject_));
    const char *styleName = "staticStyle";
    jerry_value_t styleProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(styleName));
    valueStaticStyle_ = jerry_create_object();
    jerry_release_value(jerry_set_property(globalObject_, styleProp, valueStaticStyle_));
    jerry_release_value(keyAttrs);
    jerry_release_value(styleProp);
    rootComponentMock_.PrepareRootContainer();
}

void InputTddTest::TearDown()
{
    ReleaseJerryValue(attrsObject_, globalObject_, valueStaticStyle_, VA_ARG_END_FLAG);
    JsAppContext::GetInstance()->ReleaseStyles();
}

UIView *InputTddTest::GetComponent() const
{
    uint16_t componentId = KeyParser::ParseKeyId("input");
    jerry_value_t children = UNDEFINED;
    Component *component = ComponentFactory::CreateComponent(componentId, globalObject_, children);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create input component error");
        return nullptr;
    }
    rootComponentMock_.RenderComponent(*component);
    UIView *view = component->GetComponentRootView();
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create input component error");
    }
    jerry_release_value(children);
    return view;
}

void InputTddTest::AddStringProperty(jerry_value_t object, const char *name, const char *value) const
{
    jerry_value_t nameProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    jerry_value_t valueProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_release_value(jerry_set_property(object, nameProp, valueProp));
    ReleaseJerryValue(nameProp, valueProp, VA_ARG_END_FLAG);
}

void InputTddTest::AddNumberProperty(jerry_value_t object, const char *name, const double value) const
{
    jerry_value_t nameProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    jerry_value_t valueProp = jerry_create_number(value);
    jerry_release_value(jerry_set_property(object, nameProp, valueProp));
    ReleaseJerryValue(nameProp, valueProp, VA_ARG_END_FLAG);
}

void InputTddTest::AddBoolProperty(jerry_value_t object, const char *name, bool value) const
{
    jerry_value_t nameProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    jerry_value_t valueProp = jerry_create_boolean(value);
    jerry_release_value(jerry_set_property(object, nameProp, valueProp));
    ReleaseJerryValue(nameProp, valueProp, VA_ARG_END_FLAG);
}

bool InputTddTest::CompareColor(ColorType color, uint32_t colorIntValue) const
{
    uint8_t red8 = uint8_t((colorIntValue & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
    uint8_t green8 = uint8_t((colorIntValue & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
    uint8_t blue8 = uint8_t((colorIntValue & TEXT_BLUE_COLOR_MASK));
    ColorType compare = Color::GetColorFromRGB(red8, green8, blue8);
    return (color.blue == compare.blue && color.red == compare.red && color.green == compare.green);
}

UIRadioButton *InputTddTest::ConvertView(const char *name) const
{
    UIView *view = GetComponent();
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the view is null in %s", name);
        return nullptr;
    }
    UIRadioButton *radio = reinterpret_cast<UIRadioButton *>(view);
    return radio;
}

void InputTddTest::ComponentInputAttributeTest01()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1:set the type checkbox and get the native element
     */
    const char *prop = "type";
    const char *value = "checkbox";
    AddStringProperty(attrsObject_, prop, value);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::UNSELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create checkbox, the checked attribute is error");
        return;
    }

    /**
     * @tc.steps:step2:set the checked attribute true
     * @tc.expect: the value of checked is selected
     */
    const char *checkedProp = "checked";
    AddBoolProperty(attrsObject_, checkedProp, true);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::SELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "checkbox set checked true error");
        return;
    }

    /**
     * @tc.steps: step3:set the checked attribute false
     * @tc.expect: the value of checked attribute is unselected
     */
    AddBoolProperty(attrsObject_, checkedProp, false);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::UNSELECTED) {
        HILOG_INFO(HILOG_MODULE_ACE, "Checkbox set checked false error");
    }

    /**
     * @tc.steps: step4:set the checked attribute
     * @tc.expected: the checked attribute is error
     */
    const char *stringChecked = "aaaa";
    AddStringProperty(attrsObject_, checkedProp, stringChecked);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::UNSELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Checkbox set checked attribute error");
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest01 pass");
    }
    EXPECT_TRUE(checkbox->GetState() == UICheckBox::UNSELECTED);
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest02()
{
    TDD_CASE_BEGIN();
    /**
     * @steps: step1: set the attibute type button
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);

    /**
     * @steps: step2: set the attribute value aaaa
     * @tc.expected:the value of button is aaaa
     */
    const char *value = "value";
    const char *valueProperty = "aaa";
    AddStringProperty(attrsObject_, value, valueProperty);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (strcmp(button->GetText(), valueProperty)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the attribute value of button set incorrect");
    }

    /**
     * @steps: step2: set the attribute value nullptr
     * @tc.expected: the value of button is nullptr
     */
    jerry_value_t nullProp = UNDEFINED;
    jerry_value_t valueProp = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_release_value(jerry_set_property(attrsObject_, valueProp, nullProp));
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetText()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "execute setting value null error");
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest002 passed");
    }
    EXPECT_FALSE(button->GetText());
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest04()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the type attribute button
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);
    const char *value = "value";
    const char *valueItem = "aaaa";
    AddStringProperty(attrsObject_, value, valueItem);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create button failed");
    }

    /**
     * @tc.steps:step2:set the color value 16711680
     * @tc.expected:the font color of UILabelButton is red
     */
    const char *color = "color";
    const int32_t redValue = 16711680;
    AddNumberProperty(valueStaticStyle_, color, redValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    ColorType tempColor;
    tempColor.full = button->GetLabelStyle(STYLE_TEXT_COLOR);
    if (!CompareColor(tempColor, redValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the color error");
    }

    /**
     * @tc.steps: step3:set the color value 21474836481
     * @tc.expected: the font color of UILabelButton is red
     */
    const int32_t max = 2147483648;
    AddNumberProperty(valueStaticStyle_, color, max);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    tempColor.full = button->GetLabelStyle(STYLE_TEXT_COLOR);
    if (!CompareColor(tempColor, max)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "test the value of color in button error");
    }

    /**
     * @tc.steps: step4: set the color property -21474836482
     * @tc.expected: the font color of UILabelButton is red
     */
    const int32_t min = INT_MIN;
    AddNumberProperty(valueStaticStyle_, color, min);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    tempColor.full = button->GetLabelStyle(STYLE_TEXT_COLOR);
    if (CompareColor(tempColor, min)) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest04 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "test the min value of font color error");
    }
    EXPECT_TRUE(CompareColor(tempColor, min));
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest05()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1: set the attribute type button
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create button failed");
    }

    /**
     * @tc.steps:step2:set the attribute textAlign center
     * @tc.expected: the align attribute is UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER
     */
    const char *textAlign = "textAlign";
    const char *center = "center";
    AddStringProperty(valueStaticStyle_, textAlign, center);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetAlign() != UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the center textAlign error");
    }

    /**
     * @tc.steps:step3:set the attribute textAlign right
     * @tc.expected:the align attribute is UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT
     */
    const char *right = "right";
    AddStringProperty(valueStaticStyle_, textAlign, right);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetAlign() != UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the align attribute right error");
    }

    /**
     * @tc.steps:step4:set the attribute textAlign left
     * @tc.expected: the align attribute is UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT
     */
    const char *left = "left";
    AddStringProperty(valueStaticStyle_, textAlign, left);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetAlign() != UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the textAlign attribute left error.");
    }

    /**
     * @tc.steps:step5:set the attribute textAlign aaa
     * @tc.expected:the align attribute is UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER
     */
    const char *errorValue = "aaa";
    AddStringProperty(valueStaticStyle_, textAlign, errorValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetAlign() == UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest05 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set textAlign aaa test failed");
    }
    EXPECT_TRUE(button->GetAlign() == UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest06()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1:set the type attribute checkbox
     */
    const char *type = "type";
    const char *checkboxValue = "checkbox";
    AddStringProperty(attrsObject_, type, checkboxValue);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::UNSELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "checkbox is create and the status is error");
    }

    /**
     * @tc.steps:step2:set the checked attribute true
     * @tc.expected: the status of checkbox is selected
     */
    const char *checked = "checked";
    AddBoolProperty(attrsObject_, checked, true);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::SELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the checked attribute true error");
    }

    /**
     * @tc.steps:step3:set the checked attribute false
     * @tc.expected: the status of checkbox is unselected
     */
    AddBoolProperty(attrsObject_, checked, false);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() != UICheckBox::UNSELECTED) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the checked attribute false error");
    }

    /**
     * @tc.steps: set the checked attribute aaaa
     * @tc.expected:the status of checkbox is unselected
     */
    const char *value = "aaaa";
    AddStringProperty(attrsObject_, checked, value);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetState() == UICheckBox::UNSELECTED) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest06 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the value aaaa error");
    }
    EXPECT_TRUE(checkbox->GetState() == UICheckBox::UNSELECTED);
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest07()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1:set tye type attribute button
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);
    /**
     * @tc.steps:step2:set the attribute width 50
     * @tc.expected:the width of UILabelButton is 50
     */
    const char *width = "width";
    const int widthNumber = 50;
    AddNumberProperty(valueStaticStyle_, width, widthNumber);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetWidth() != widthNumber) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the width attribute failed");
    }

    /**
     * @tc.steps:step3:set the attribute height 20
     * @tc.expected:the height of UILabelButton is 20
     */
    const char *height = "height";
    const int heightValue = 20;
    AddNumberProperty(valueStaticStyle_, height, heightValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetHeight() != heightValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute height error");
    }

    /**
     * @tc.steps:step6:set the attribute id button1
     * @tc.expected: the id of button is button1
     */
    const char *id = "id";
    const char *idValue = "button1";
    AddStringProperty(attrsObject_, id, idValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (!strcmp(button->GetViewId(), idValue)) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest07 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute id error");
    }
    EXPECT_TRUE(!strcmp(button->GetViewId(), idValue));
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest08()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1:set the type button
     */
    const char *type = "type";
    const char *buttonValue = "button";
    AddStringProperty(attrsObject_, type, buttonValue);
    /**
     * @tc.steps: step2: set the margin 100
     * @tc.expected: the value of margin is 100
     */
    const char *margin = "margin";
    const int marginValue = 100;
    AddNumberProperty(valueStaticStyle_, margin, marginValue);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    int16_t left = button->GetStyle(STYLE_MARGIN_LEFT);
    int16_t right = button->GetStyle(STYLE_MARGIN_RIGHT);
    int16_t top = button->GetStyle(STYLE_MARGIN_TOP);
    int16_t bottom = button->GetStyle(STYLE_MARGIN_BOTTOM);
    if (!(left == marginValue && right == marginValue && top == marginValue && bottom == marginValue)) {
        HILOG_INFO(HILOG_MODULE_ACE, "set the attribute margin failed");
    }
    /**
     * @tc.steps:step3:set the attribute marginLeft 10
     * @tc.expected:the value of marginLeft is 10
     */
    const char *marginLeft = "marginLeft";
    const int marginLeftValue = 10;
    AddNumberProperty(valueStaticStyle_, marginLeft, marginLeftValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    left = button->GetStyle(STYLE_MARGIN_LEFT);
    if (left != marginLeftValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginLeft error");
    }
    TDD_CASE_END();
    ComponentInputStyleTest08Extra(right, top, bottom);
}

void InputTddTest::ComponentInputStyleTest08Extra(int16_t right, int16_t top, int16_t bottom)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step4:set the attribute marginButtom 10
     * @tc.expected: the attribute of marginButton 10
     */
    const char *marginButtom = "marginBottom";
    const int marginButtomValue = 10;
    AddNumberProperty(valueStaticStyle_, marginButtom, marginButtomValue);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    bottom = button->GetStyle(STYLE_MARGIN_BOTTOM);
    if (bottom != marginButtomValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginButtom failed");
    }
    /**
     * @tc.steps: step5: set the attribute marginRight 10
     * @tc.expected: the value of marginRight is 10
     */
    const char *marginRight = "marginRight";
    const int marginRightValue = 10;
    AddNumberProperty(valueStaticStyle_, marginRight, marginRightValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    right = button->GetStyle(STYLE_MARGIN_RIGHT);
    if (right != marginRightValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginRight failed");
    }
    /**
     * @tc.steps: step6: set the attribute marginTop 10
     * @tc.expected: the attribute marginTop is 10
     */
    const char *marginTop = "marginTop";
    const int marginTopValue = 10;
    AddNumberProperty(valueStaticStyle_, marginTop, marginTopValue);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    top = button->GetStyle(STYLE_MARGIN_TOP);
    if (top != marginTopValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginTop failed");
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest08 passed");
    }
    EXPECT_FALSE(top != marginTopValue);
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest09()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the attribute button
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);

    /**
     * @tc.steps: step2:set the borderWidth 2
     * @tc.expected:the border width of button is 2
     */
    const char *borderWidth = "borderWidth";
    const int borderWidthValue = 2;
    AddNumberProperty(valueStaticStyle_, borderWidth, borderWidthValue);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetStyle(STYLE_BORDER_WIDTH) != borderWidthValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the borderWidth failed");
        return;
    }

    /**
     * @tc.steps: step3:set the borderColor red
     * @tc.expected: the borderColor of UILabelButton is red
     */
    const char *borderColor = "borderColor";
    const int color = 16711680;
    AddNumberProperty(valueStaticStyle_, borderColor, color);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    ColorType tempColor;
    tempColor.full = button->GetStyle(STYLE_BORDER_COLOR);
    if (!CompareColor(tempColor, color)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute border color failed");
    }

    /**
     * @tc.step: step4:set the radius 1
     * @tc.expected: the attribute of radius is 1
     */
    const char *radius = "borderRadius";
    const int radiusNum = 1;
    AddNumberProperty(valueStaticStyle_, radius, radiusNum);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    if (button->GetStyle(STYLE_BORDER_RADIUS) != 1) {
        HILOG_INFO(HILOG_MODULE_ACE, "set the radius failed");
    }

    /**
     * @tc.steps: step5:set the attribute backgroundColor red
     * @tc.expected: the backgroundColor of UILabelButton is red
     */
    const char *backgroundColor = "backgroundColor";
    AddNumberProperty(valueStaticStyle_, backgroundColor, color);
    button = reinterpret_cast<UILabelButton *>(GetComponent());
    tempColor.full = button->GetStyle(STYLE_BACKGROUND_COLOR);
    if (CompareColor(tempColor, color)) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest09 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute backgroundColor failed");
    }
    EXPECT_TRUE(CompareColor(tempColor, color));
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest10()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the attribute button
     */
    const char *type = "type";
    const char *typeValue = "checkbox";
    AddStringProperty(attrsObject_, type, typeValue);

    /**
     * @tc.steps:step2:set the attribute width 50
     * @tc.expected: the width of checkbox is 50
     */
    const char *width = "width";
    const int widthValue = 50;
    AddNumberProperty(valueStaticStyle_, width, widthValue);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetWidth() != widthValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute of width failed");
    }

    /**
     * @tc.steps:step3:set the attribute height 20
     * @tc.expected: the height of UILabelButton is 20
     */
    const char *height = "height";
    const int heightValue = 20;
    AddNumberProperty(valueStaticStyle_, height, heightValue);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetHeight() != heightValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute height failed");
    }

    /**
     * @tc.steps: step4:set the attribute id checkbox1
     * @tc.expected: the attribute id of checkbox is checkbox1
     */
    const char *id = "id";
    const char *idValue = "button1";
    AddStringProperty(attrsObject_, id, idValue);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (!strcmp(idValue, checkbox->GetViewId())) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest10 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Set the attribute id of checkbox failed");
    }
    EXPECT_TRUE(!strcmp(idValue, checkbox->GetViewId()));
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest11()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1: set the attribute type checkbox
     */
    const char *type = "type";
    const char *typeValue = "checkbox";
    AddStringProperty(attrsObject_, type, typeValue);
    /**
     * @tc.steps:step2:set the attribute margin 10
     * @tc.expected:the attribute margin of checkbox is 10
     */
    const char *margin = "margin";
    const int marginValue = 100;
    AddNumberProperty(valueStaticStyle_, margin, marginValue);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    int16_t left = checkbox->GetStyle(STYLE_MARGIN_LEFT);
    int16_t right = checkbox->GetStyle(STYLE_MARGIN_RIGHT);
    int16_t top = checkbox->GetStyle(STYLE_MARGIN_TOP);
    int16_t bottom = checkbox->GetStyle(STYLE_MARGIN_BOTTOM);
    if (!(left == marginValue && right == marginValue && top == marginValue && bottom == marginValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute margin failed");
    }
    /**
     * @tc.steps:step3:set the attribute marginLeft 10
     * @tc.expected: the attribute margin left is 10
     */
    const char *marginLeft = "marginLeft";
    const int value = 20;
    AddNumberProperty(valueStaticStyle_, marginLeft, value);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    left = checkbox->GetStyle(STYLE_MARGIN_LEFT);
    if (left != value) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginLeft failed");
    }
    /**
     * @tc.steps: step4: set the attribute marginRight 10
     * @tc.expected: the value of marginRight is 10
     */
    const char *marginRight = "marginRight";
    AddNumberProperty(valueStaticStyle_, marginRight, value);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    right = checkbox->GetStyle(STYLE_MARGIN_RIGHT);
    if (right != value) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginRight failed");
    }
    /**
     * @tc.steps:step5:set the attribute marginTop 10
     * @tc.expected: the marginRight attribute of checkbox is 10
     */
    const char *marginTop = "marginTop";
    AddNumberProperty(valueStaticStyle_, marginTop, value);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    top = checkbox->GetStyle(STYLE_MARGIN_TOP);
    if (top != value) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginTop error");
    }
    /**
     * @tc.steps: step6:set the attribute marginButtom 10
     * @tc.expected: the attribute marginButtom of checkbox is 10
     */
    const char *marginButtom = "marginBottom";
    AddNumberProperty(valueStaticStyle_, marginButtom, value);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    bottom = checkbox->GetStyle(STYLE_MARGIN_BOTTOM);
    if (bottom == value) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest11 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute marginButtom failed");
    }
    EXPECT_TRUE(bottom == value);
    TDD_CASE_END();
}

void InputTddTest::ComponentInputStyleTest12()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the attribute type checkbox
     */
    const char *type = "type";
    const char *typeValue = "checkbox";
    AddStringProperty(attrsObject_, type, typeValue);

    /**
     * @tc.steps: step2:set the attribute borderWidth 2
     * @tc.expected: the borderWidth of checkbox is 2
     */
    const char *borderWidth = "borderWidth";
    const int borderWidthValue = 2;
    AddNumberProperty(valueStaticStyle_, borderWidth, borderWidthValue);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetStyle(STYLE_BORDER_WIDTH) != borderWidthValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the style borderWidth failed");
    }

    /**
     * @tc.steps: step3:set the attribute borderColor red
     * @tc.expected: the borderColor of checkbox is red
     */
    const char *borderColor = "borderColor";
    const int color = 16711680;
    AddNumberProperty(valueStaticStyle_, borderColor, color);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    ColorType tempColor;
    tempColor.full = checkbox->GetStyle(STYLE_BORDER_COLOR);
    if (!CompareColor(tempColor, color)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute borderColor of checkbox failed");
    }

    /**
     * @tc.steps: step4:set the attribute radius 1
     * @tc.expected:the style radius of checkbox is 1
     */
    const char *radius = "borderRadius";
    const int radiusValue = 1;
    AddNumberProperty(valueStaticStyle_, radius, radiusValue);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    if (checkbox->GetStyle(STYLE_BORDER_RADIUS) != radiusValue) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute radius failed");
    }

    /**
     * @tc.steps:step5:set the backgroundColor red
     * @tc.expected: the backgroundColor of checkbox is red
     */
    const char *backgroundColor = "backgroundColor";
    AddNumberProperty(valueStaticStyle_, backgroundColor, color);
    checkbox = reinterpret_cast<UICheckBox *>(GetComponent());
    tempColor.full = checkbox->GetStyle(STYLE_BACKGROUND_COLOR);
    if (CompareColor(tempColor, color)) {
        HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputStyleTest12 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute backgroundColor failed");
    }
    EXPECT_TRUE(CompareColor(tempColor, color));
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest13()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the attribute type button and set the attribute checked true
     */
    const char *type = "type";
    const char *typeValue = "button";
    AddStringProperty(attrsObject_, type, typeValue);
    const char *checked = "checked";
    AddBoolProperty(attrsObject_, checked, false);

    /**
     * @tc.steps:step2:get the component
     * @tc.expected: the type of component is button
     */
    UIView *view = GetComponent();
    if (view) {
        UILabelButton *button = reinterpret_cast<UILabelButton *>(view);
        if (button == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create the view failed");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "check the checked attribute in button error");
    }

    /**
     * @tc.steps:step3:set the attribute type checkbox and set the attribute value aaa
     */
    const char *checkboxValue = "checkbox";
    AddStringProperty(attrsObject_, type, checkboxValue);
    const char *value = "value";
    const char *testValue = "aaa";
    AddStringProperty(attrsObject_, value, testValue);

    /**
     * @tc.steps:step4:get the component
     * @tc.expected: the type of component is checkbox
     */
    view = GetComponent();
    if (view) {
        UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(view);
        if (checkbox == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create view failed");
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest13 passed");
        }
        EXPECT_FALSE(checkbox == nullptr);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute value in checkbox failed");
        EXPECT_FALSE(true);
    }
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest14()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1:set the attribute type button
     * @tc.expected: the class name of component is UILabelButton
     */
    const char *type = "type";
    const char *button = "button";
    AddStringProperty(attrsObject_, type, button);
    UIView *view = GetComponent();
    if (view) {
        UILabelButton *button = reinterpret_cast<UILabelButton *>(view);
        if (button == nullptr) {
            HILOG_INFO(HILOG_MODULE_ACE, "Create the error view");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute type button failed");
    }

    /**
     * @tc.steps:step2:set the attribute type checkbox
     * @tc.expected: the class name of component is UICheckbox
     */
    const char *checkbox = "checkbox";
    AddStringProperty(attrsObject_, type, checkbox);
    view = GetComponent();
    if (view) {
        UILabelButton *button = reinterpret_cast<UILabelButton *>(view);
        if (button == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Create the error view");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the type attribute checkbox failed");
    }

    /**
     * @tc.steps:step3:get the component object
     * @tc.expected: the className of component is UILabelButton
     */
    view = GetComponent();
    if (view) {
        UILabelButton *button = reinterpret_cast<UILabelButton *>(view);
        if (button == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create the error view");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "the attribute type is error");
    }

    /**
     * @tc.steps:step4:set the type value aaaa
     * @tc.expected: the type of component is UILabelButton
     */
    const char *testTypeValue = "aaaa";
    AddStringProperty(attrsObject_, type, testTypeValue);
    view = GetComponent();
    if (view) {
        UILabelButton *button = reinterpret_cast<UILabelButton *>(view);
        if (button == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create the error view");
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest14 passed");
        }
        EXPECT_FALSE(checkbox == nullptr);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the type attribute aaa failed");
        EXPECT_FALSE(true);
    }
    TDD_CASE_END();
}

void InputTddTest::ComponentInputAttributeTest15()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1:set the type checkbox
     */
    const char *type = "type";
    const char *typeValue = "checkbox";
    AddStringProperty(attrsObject_, type, typeValue);

    /**
     * @tc.steps:step2:set the fontSize mini
     * @tc.expected:the className of view is UICheckbox
     */
    const char *fontSize = "fontSize";
    const char *fontSizeMini = "mini";
    AddStringProperty(valueStaticStyle_, fontSize, fontSizeMini);
    UIView *view = GetComponent();
    if (view) {
        UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(view);
        if (checkbox == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create view error");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "check the fontSize in checkbox failed");
    }

    /**
     * @tc.steps:step3:set the color red
     * @tc.expected: the name of component is UICheckbox
     */
    const char *color = "color";
    const int colorValue = 16711680;
    AddNumberProperty(valueStaticStyle_, color, colorValue);
    view = GetComponent();
    if (view) {
        UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(view);
        if (checkbox == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create view failed");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "use attribute color in checkbox failed");
    }

    /**
     * @tc.steps: step4:set the attribute textAlign
     * @tc.expected: the class name of text
     */
    const char *textAlign = "textAlign";
    const char *textAlignLeft = "left";
    AddStringProperty(valueStaticStyle_, textAlign, textAlignLeft);
    view = GetComponent();
    if (view) {
        UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(view);
        if (checkbox == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create the error type view");
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "ComponentInputAttributeTest15 passed");
        }
        EXPECT_FALSE(checkbox == nullptr);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the textAlign in checkbox failed");
        EXPECT_FALSE(true);
    }
    TDD_CASE_END();
}

void InputTddTest::ComponentRadioAttributeTest01()
{
    TDD_CASE_BEGIN();
    /**
     * @steps:step1:set the type attribute radio
     * @expected: create UIRadioButton
     */
    const char *type = "type";
    const char *typeVal = "radio";
    AddStringProperty(attrsObject_, type, typeVal);

    /**
     * @steps:step2:set the attribute checked true
     * @expected:the state of radioButton is selected
     */
    const char *checked = "checked";
    AddBoolProperty(attrsObject_, checked, true);
    const char *tddName = "ComponentRadioAttributeTest01";
    UIRadioButton *radio = ConvertView(tddName);
    if (radio == nullptr || (radio->GetState() != UICheckBox::UICheckBoxState::SELECTED)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute checked false");
        return;
    }

    /**
     * @steps:step3:set the attribute checked false
     * @expected:the state of radioButton is unselected
     */
    AddBoolProperty(attrsObject_, checked, false);
    radio = ConvertView(tddName);
    if (radio == nullptr || (radio->GetState() != UICheckBox::UICheckBoxState::UNSELECTED)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute checked false failed");
        return;
    }

    /**
     * @steps: step4:set the attribute checked aaaa
     * @expected: the state of radio is unselected
     */
    const char *checkedVal = "aaaa";
    AddStringProperty(attrsObject_, checked, checkedVal);
    radio = ConvertView(tddName);
    if (radio != nullptr && (radio->GetState() == UICheckBox::UICheckBoxState::UNSELECTED)) {
        HILOG_INFO(HILOG_MODULE_ACE, "Component_Radio_Attribute_Test_01 passed");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the checked attribute aaaa failed");
    }
    EXPECT_TRUE(radio != nullptr && (radio->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    TDD_CASE_END();
}

void InputTddTest::ComponentRadioStyleTest03()
{
    TDD_CASE_BEGIN();
    /**
     * @steps:step1:set the attribute type radio
     */
    const char *type = "type";
    const char *typeVal = "radio";
    AddStringProperty(attrsObject_, type, typeVal);

    /**
     * @steps: step2: set the radio border width 2
     * @expected: the border width of radio is 2
     */
    const int borderWidthVal = 2;
    const char *borderWidth = "borderWidth";
    AddNumberProperty(valueStaticStyle_, borderWidth, borderWidthVal);
    const char *tddName = "ComponentRadioStyleTest03";
    UIRadioButton *radioButton = ConvertView(tddName);
    if (radioButton != nullptr) {
        Style style = radioButton->GetStyleConst();
        if (style.borderWidth_ != borderWidthVal) {
            HILOG_ERROR(HILOG_MODULE_ACE, "set the border width failed");
            return;
        }
    } else {
        return;
    }

    /**
     * @steps:step3: set the radius 100
     * @expected: the radius of radioButton is 100
     */
    const char *radius = "borderRadius";
    const int radiusVal = 100;
    AddNumberProperty(valueStaticStyle_, radius, radiusVal);
    radioButton = ConvertView(tddName);
    if (radioButton != nullptr) {
        Style style = radioButton->GetStyleConst();
        if (style.borderRadius_ != radiusVal) {
            HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute radius failed");
            return;
        }
    } else {
        return;
    }

    /**
     * @steps: step4: set the background color of radioButton red
     * @expected: the background color of radiobutton is red
     */
    const int32_t redValue = 16711680;
    const char *backgroundColor = "backgroundColor";
    AddNumberProperty(valueStaticStyle_, backgroundColor, redValue);
    radioButton = ConvertView(tddName);
    if (radioButton != nullptr) {
        Style style = radioButton->GetStyleConst();
        if (!CompareColor(style.bgColor_, redValue)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "set the background color failed");
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "Component_Radio_Style_Test_03 passed");
        }
        EXPECT_FALSE(!CompareColor(style.bgColor_, redValue));
    } else {
        return;
    }
    TDD_CASE_END();
}

void InputTddTest::ComponentRadioStyleTest04()
{
    TDD_CASE_BEGIN();
    /**
     * @steps:step1:set the attribute type radio
     */
    const char *type = "type";
    const char *typeVal = "radio";
    AddStringProperty(attrsObject_, type, typeVal);

    /**
     * @steps: step2: set the radio width 50
     * @expected: the width of radio is 50
     */
    const int widthVal = 50;
    const char *width = "width";
    AddNumberProperty(valueStaticStyle_, width, widthVal);
    const char *tddName = "ComponentRadioStyleTest04";
    UIRadioButton *radioButton = ConvertView(tddName);
    if (radioButton == nullptr || (radioButton->GetWidth() != widthVal)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the border width failed");
        return;
    }
    /**
     * @steps:step3: set the radiobutton height 50
     * @expected: the height of radioButton is 50
     */
    const char *height = "height";
    const int heightVal = 50;
    AddNumberProperty(valueStaticStyle_, height, heightVal);
    radioButton = ConvertView(tddName);
    if (radioButton == nullptr || (radioButton->GetHeight() != heightVal)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the attribute height failed");
        return;
    }
    /**
     * @steps: step4: set the attribute id radio1
     * @expected: the id of radioButton is radio1
     */
    const char *id = "id";
    const char *idVal = "radio1";
    AddStringProperty(attrsObject_, id, idVal);
    radioButton = ConvertView(tddName);
    if (radioButton == nullptr || (strcmp(radioButton->GetViewId(), idVal))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the background color failed");
        return;
    }
    /**
     * @steps:step5:set the attribute visible false
     * @expected: the visible of radiobutton is false
     */
    const char *show = "show";
    AddBoolProperty(attrsObject_, show, false);
    radioButton = ConvertView(tddName);
    if (radioButton == nullptr || (!radioButton->IsVisible())) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Component_Radio_Style_Test_04 passed");
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "set the attribute visible false");
    }
    EXPECT_TRUE(radioButton == nullptr || (!radioButton->IsVisible()));
    TDD_CASE_END();
}

void InputTddTest::RunTests()
{
    ComponentInputAttributeTest01();
    ComponentInputAttributeTest02();
    ComponentInputStyleTest04();
    ComponentInputStyleTest05();
    ComponentInputAttributeTest06();
    ComponentInputAttributeTest07();
    ComponentInputStyleTest08();
    ComponentInputStyleTest09();
    ComponentInputAttributeTest10();
    ComponentInputStyleTest11();
    ComponentInputStyleTest12();
    ComponentInputAttributeTest13();
    ComponentInputAttributeTest14();
    ComponentInputAttributeTest15();
    ComponentRadioAttributeTest01();
    ComponentRadioStyleTest03();
    ComponentRadioStyleTest04();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: Component_Input_Attribute_Test_01
 * @tc.desc: test the attribute checked in checkbox is correct
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr001, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest01();
}

/**
 * @tc.name: Component_Input_Attribute_Test_02
 * @tc.desc: test the attribute value
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr002, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest02();
}

/**
 * @tc.name: Component_Input_Attribute_Test_06
 * @tc.desc: test the attribute checkbox
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr006, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest06();
}

/**
 * @tc.name: Component_Input_Attribute_Test_07
 * @tc.desc: check the general attribute could work or not
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr007, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest07();
}

/**
 * @tc.name: Component_Input_Attribute_Test_10
 * @tc.desc: check the general attribute of checkbox could work or not
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr010, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest10();
}

/**
 * @tc.name: Component_Input_Attribute_Test_13
 * @tc.desc: test the attribute value in checkbox and the attribute checked in button
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr013, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest13();
}

/**
 * @tc.name: Component_Input_Attribute_Test_14
 * @tc.desc: test the attribute type
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr014, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest14();
}

/**
 * @tc.name: Component_Input_Attribute_Test_15
 * @tc.desc: when checkbox is created, fontSize, color,textAlign could work
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputAttr015, TestSize.Level1)
{
    InputTddTest::ComponentInputAttributeTest15();
}

/**
 * @tc.name: Component_Input_Style_Test_04
 * @tc.desc: test the style color
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle004, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest04();
}

/**
 * @tc.name: Component_Input_Style_Test_05
 * @tc.desc: test the style textAlign
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle005, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest05();
}

/**
 * @tc.name: Component_Input_Style_Test_08
 * @tc.desc: check the general style could work or not
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle008, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest08();
}

/**
 * @tc.name: Component_Input_Style_Test_09
 * @tc.desc: check the general style could work or not
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle009, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest09();
}

/**
 * @tc.name: Component_Input_Style_Test_11
 * @tc.desc: check the general style of checkbox could work or not
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle011, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest11();
}

/**
 * @tc.name: Component_Input_Style_Test_12
 * @tc.desc: check the general style of checkbox could work or not
 * @tc.type:FUNC
 * @tc.require: AR000DSEFK
 */
HWTEST_F(InputTddTest, inputStyle012, TestSize.Level1)
{
    InputTddTest::ComponentInputStyleTest12();
}

/**
 * @tc.name: Component_Radio_Attribute_Test_01
 * @tc.desc: test the attribute checked
 * @tc.type:FUNC
 * @tc.require: AR000E2TCH
 */
HWTEST_F(InputTddTest, radioAttr001, TestSize.Level1)
{
    InputTddTest::ComponentRadioAttributeTest01();
}

/**
 * @tc.name: Component_Radio_Style_Test_03
 * @tc.desc: test the common style
 * @tc.type:FUNC
 * @tc.require: AR000E2TCH
 */
HWTEST_F(InputTddTest, radioStyle003, TestSize.Level1)
{
    InputTddTest::ComponentRadioStyleTest03();
}

/**
 * @tc.name: Component_Radio_Style_Test_04
 * @tc.desc: test the common attribute
 * @tc.type:FUNC
 * @tc.require: AR000E2TCH
 */
HWTEST_F(InputTddTest, radioStyle004, TestSize.Level1)
{
    InputTddTest::ComponentRadioStyleTest04();
}
#endif
}
}
