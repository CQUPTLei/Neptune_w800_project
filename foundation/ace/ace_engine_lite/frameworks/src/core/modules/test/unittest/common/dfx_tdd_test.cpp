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
#include "dfx_tdd_test.h"
#include <unistd.h>
#include "js_fwk_common.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
const char * const DfxTddTest::BUNDLE1
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680},"
                          "                  on : {'click' : _vm.click1}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     var sum = num + 1;"
                          "     this.textValue = 'Hello Ace';"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE2
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     this.click4();"
                          "     this.textValue = 'Hello Ace';"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE3
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     this.textValue = 'Hello Ace';"
                          "     console.log('Hello Ace.');"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE4
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click32: function click32() {"
                          "     this,click33();"
                          "   },"
                          "   click31: function click31() {"
                          "     this.click32();"
                          "   },"
                          "   click30: function click30() {"
                          "     this.click31();"
                          "   },"
                          "   click29: function click29() {"
                          "     this,click30();"
                          "   },"
                          "   click28: function click28() {"
                          "     this.click29();"
                          "   },"
                          "   click27: function click27() {"
                          "     this.click28();"
                          "   },"
                          "   click26: function click26() {"
                          "     this,click27();"
                          "   },"
                          "   click25: function click25() {"
                          "     this.click26();"
                          "   },"
                          "   click24: function click24() {"
                          "     this,click25();"
                          "   },"
                          "   click23: function click23() {"
                          "     this.click24();"
                          "   },"
                          "   click22: function click22() {"
                          "     this.click23();"
                          "   },"
                          "   click21: function click21() {"
                          "     this,click22();"
                          "   },"
                          "   click20: function click20() {"
                          "     this.click21();"
                          "   },"
                          "   click19: function click19() {"
                          "     this.click20();"
                          "   },"
                          "   click18: function click18() {"
                          "     this,click19();"
                          "   },"
                          "   click17: function click17() {"
                          "     this.click18();"
                          "   },"
                          "   click16: function click16() {"
                          "     this.click17();"
                          "   },"
                          "   click15: function click15() {"
                          "     this,click16();"
                          "   },"
                          "   click14: function click14() {"
                          "     this.click15();"
                          "   },"
                          "   click13: function click13() {"
                          "     this.click14();"
                          "   },"
                          "   click12: function click12() {"
                          "     this,click13();"
                          "   },"
                          "   click11: function click11() {"
                          "     this.click12();"
                          "   },"
                          "   click10: function click10() {"
                          "     this.click11();"
                          "   },"
                          "   click9: function click9() {"
                          "     this,click10();"
                          "   },"
                          "   click8: function click8() {"
                          "     this.click9();"
                          "   },"
                          "   click7: function click7() {"
                          "     this.click8();"
                          "   },"
                          "   click6: function click6() {"
                          "     this,click7();"
                          "   },"
                          "   click5: function click5() {"
                          "     this.click6();"
                          "   },"
                          "   click4: function click4() {"
                          "     this.click5();"
                          "   },"
                          "   click3: function click3() {"
                          "     this,click4();"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::FUNC_NAME = "click1";

void DfxTddTest::DfxTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (!jerry_value_is_error(ret)) {
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }

    // trace out error information if the result contains error
    jerry_value_t errValue = jerry_get_value_from_error(ret, false);
    jerry_value_t errStrVal = jerry_value_to_string(errValue);
    jerry_release_value(errValue);
    jerry_size_t errStrSize = jerry_get_utf8_string_size(errStrVal);
    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (errStrSize + 1)));
    if (errStrBuffer == nullptr) {
        jerry_release_value(errStrVal);
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(errStrVal, errStrBuffer, errStrSize);
    errStrBuffer[stringEnd] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(errStrBuffer), "ReferenceError: num is not defined");
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    jerry_release_value(errStrVal);

    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello World");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    jerry_release_value(value);
    TDD_CASE_END();
    DestroyPage(page);
}

void DfxTddTest::DfxTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));
    JSValue errorValue = JSObject::Call(page, FUNC_NAME);
    if (!jerry_value_is_error(errorValue)) {
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    const uint16_t stackMsgMaxLength = 256;
    const uint8_t exceptLength = 3;
    const char *stack = "stack";
    jerry_value_t stackStr = jerry_create_string((const jerry_char_t *) stack);
    jerry_value_t errorVal = jerry_get_value_from_error(errorValue, false);
    jerry_value_t backtraceVal = jerry_get_property(errorVal, stackStr);
    ReleaseJerryValue(stackStr, errorVal, VA_ARG_END_FLAG);
    if (jerry_value_is_error(backtraceVal) || !(jerry_value_is_array(backtraceVal))) {
        TDD_CASE_END();
        DestroyPage(page);
        jerry_release_value(backtraceVal);
        return;
    }
    uint32_t length = jerry_get_array_length(backtraceVal);
    EXPECT_EQ(length, exceptLength);
    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (stackMsgMaxLength)));
    if (errStrBuffer == nullptr) {
        jerry_release_value(backtraceVal);
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    jerry_value_t itemVal = jerry_get_property_by_index(backtraceVal, 0);
    jerry_size_t strSize = 0;
    if (!jerry_value_is_error(itemVal) && jerry_value_is_string(itemVal)) {
        strSize = jerry_get_utf8_string_size(itemVal);
    }
    jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(itemVal, errStrBuffer, strSize);
    errStrBuffer[stringEnd] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(errStrBuffer), "click3(), ");
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello World");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    ReleaseJerryValue(itemVal, backtraceVal, value, VA_ARG_END_FLAG);
    TDD_CASE_END();
    DestroyPage(page);
}

void DfxTddTest::DfxTest003()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE3, strlen(BUNDLE3));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (jerry_value_is_error(ret)) {
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello Ace");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    jerry_release_value(value);
    TDD_CASE_END();
    DestroyPage(page);
}

void DfxTddTest::DfxTest004()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE4, strlen(BUNDLE4));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (jerry_value_is_error(ret)) {
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    const uint8_t exceptLength = 32;
    const char *stack = "stack";

    jerry_value_t stackStr = jerry_create_string((const jerry_char_t *) stack);
    jerry_value_t errorVal = jerry_get_value_from_error(ret, false);

    jerry_value_t backtraceVal = jerry_get_property(errorVal, stackStr);
    jerry_release_value(stackStr);
    jerry_release_value(errorVal);

    if (jerry_value_is_error(backtraceVal) || !(jerry_value_is_array(backtraceVal))) {
        TDD_CASE_END();
        DestroyPage(page);
        jerry_release_value(backtraceVal);
        return;
    }

    uint32_t length = jerry_get_array_length(backtraceVal);
    EXPECT_EQ(length, exceptLength);
    TDD_CASE_END();
    jerry_release_value(backtraceVal);
    DestroyPage(page);
}


void DfxTddTest::RunTests()
{
    DfxTest001();
    DfxTest002();
    DfxTest003();
    DfxTest004();
}

#ifdef TDD_ASSERTIONS
/* *
 * @tc.name: DfxTest001
 * @tc.desc: Verify error code.
 * @tc.require: AR000F3PDP
 */
HWTEST_F(DfxTddTest, DfxTest001, TestSize.Level1)
{
    DfxTddTest::DfxTest001();
}

/* *
 * @tc.name: DfxTest002
 * @tc.desc: Verify error message.
 * @tc.require: AR000F3PDP
 */
HWTEST_F(DfxTddTest, DfxTest002, TestSize.Level1)
{
    DfxTddTest::DfxTest002();
}

/* *
 * @tc.name: DfxTest003
 * @tc.desc: Verify normal process.
 * @tc.require: AR000F3PDP
 */
HWTEST_F(DfxTddTest, DfxTest003, TestSize.Level1)
{
    DfxTddTest::DfxTest003();
}

/* *
 * @tc.name: DfxTest004
 * @tc.desc: Verify pressure test.
 * @tc.require: AR000F3PDP
 */
HWTEST_F(DfxTddTest, DfxTest004, TestSize.Level1)
{
    DfxTddTest::DfxTest004();
}
#endif // TDD_ASSERTIONS
}
} // namespace ACELite
