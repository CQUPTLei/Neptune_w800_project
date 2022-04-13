/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <stdio.h>
#include <stdlib.h>
#include "ohos_errno.h"
#include "parameter.h"

using namespace testing::ext;

namespace OHOS {
class ParameterTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        printf("----------test case with parameter start-------------\n");
    }
    static void TearDownTestCase()
    {
        printf("----------test case with parameter end---------------\n");
    }
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(ParameterTest, parameterTest001, TestSize.Level0)
{
    char* value1 = GetProductType();
    printf("Product type =%s\n", value1);
    free(value1);
    char* value2 = GetManufacture();
    printf("Manufacture =%s\n", value2);
    free(value2);
    char* value3 = GetBrand();
    printf("GetBrand =%s\n", value3);
    free(value3);
    char* value4 = GetMarketName();
    printf("MarketName =%s\n", value4);
    free(value4);
    char* value5 = GetProductSeries();
    printf("ProductSeries =%s\n", value5);
    free(value5);
    char* value6 = GetProductModel();
    printf("ProductModel =%s\n", value6);
    free(value6);
    char* value7 = GetSoftwareModel();
    printf("SoftwareModel =%s\n", value7);
    free(value7);
    char* value8 = GetHardwareModel();
    printf("HardwareModel =%s\n", value8);
    free(value8);
    char* value9 = GetHardwareProfile();
    printf("Software profile =%s\n", value9);
    free(value9);
    char* value10 = GetSerial();
    printf("Serial =%s\n", value10);
    free(value10);
    char* value11 = GetOsName();
    printf("OS name =%s\n", value11);
    free(value11);
    char* value12 = GetDisplayVersion();
    printf("Display version =%s\n", value12);
    free(value12);
    char* value13 = GetBootloaderVersion();
    printf("bootloader version =%s\n", value13);
    free(value13);
    char* value14 = GetSecurityPatchTag();
    printf("secure patch level =%s\n", value14);
    free(value14);
}

HWTEST_F(ParameterTest, parameterTest001_1, TestSize.Level0)
{
    char* value14 = GetSecurityPatchTag();
    printf("secure patch level =%s\n", value14);
    free(value14);
    char* value15 = GetAbiList();
    printf("abi list =%s\n", value15);
    free(value15);
    char* value16 = GetFirstApiLevel();
    printf("first api level =%s\n", value16);
    free(value16);
    char* value17 = GetIncrementalVersion();
    printf("Incremental version = %s\n", value17);
    free(value17);
    char* value187 = GetVersionId();
    printf("formal id =%s\n", value187);
    free(value187);
    char* value18 = GetBuildType();
    printf("build tyep =%s\n", value18);
    free(value18);
    char* value19 = GetBuildUser();
    printf("build user =%s\n", value19);
    free(value19);
    char* value20 = GetBuildHost();
    printf("Build host = %s\n", value20);
    free(value20);
    char* value21 = GetBuildTime();
    printf("build time =%s\n", value21);
    free(value21);
    char* value22 = GetBuildRootHash();
    printf("build root later..., %s\n", value22);
    free(value22);
}

HWTEST_F(ParameterTest, parameterTest002, TestSize.Level0)
{
    char key1[] = "ro.sys.version";
    char value1[] = "set read only key";
    int ret = SetParameter(key1, value1);
    EXPECT_EQ(ret, EC_INVALID);
}

HWTEST_F(ParameterTest, parameterTest003, TestSize.Level0)
{
    char key2[] = "rw.sys.version*%version";
    char value2[] = "set value with illegal key";
    int ret = SetParameter(key2, value2);
    EXPECT_EQ(ret, EC_INVALID);
}

/* key = 32 */
HWTEST_F(ParameterTest, parameterTest004, TestSize.Level0)
{
    char key3[] = "rw.sys.version.utilskvparameter0";
    char value3[] = "set with key = 32";
    int ret = SetParameter(key3, value3);
    EXPECT_EQ(ret, EC_INVALID);
}

/* value > 128 */
HWTEST_F(ParameterTest, parameterTest005, TestSize.Level0)
{
    char key4[] = "rw.sys.version.version";
    char value4[] = "rw.sys.version.version.version.version flash_offset = *(hi_u32 *)DT_SetGetU32(&g_Element[0], 0);\
    size = *(hi_u32 *)DT_SetGetU32(&g_Element[1], 0);";
    int ret = SetParameter(key4, value4);
    EXPECT_EQ(ret, EC_INVALID);
}

HWTEST_F(ParameterTest, parameterTest006, TestSize.Level0)
{
    char key1[] = "rw.product.not.exist";
    char value1[64] = {0};
    char defValue1[] = "value of key not exist...";
    int ret = GetParameter(key1, defValue1, value1, 64);
    EXPECT_EQ(ret, strlen(defValue1));
}

HWTEST_F(ParameterTest, parameterTest007, TestSize.Level0)
{
    char key2[] = "rw.sys.version.version.version.version";
    char value2[64] = {0};
    char defValue2[] = "value of key > 32 ...";
    int ret = GetParameter(key2, defValue2, value2, 64);
    EXPECT_EQ(ret, EC_INVALID);
}

HWTEST_F(ParameterTest, parameterTest008, TestSize.Level0)
{
    char key4[] = "rw.sys.version";
    char* value4 = nullptr;
    char defValue3[] = "value of key > 32 ...";
    int ret = GetParameter(key4, defValue3, value4, 0);
    EXPECT_EQ(ret, EC_INVALID);
}

HWTEST_F(ParameterTest, parameterTest009, TestSize.Level0)
{
    char key5[] = "rw.product.type";
    char value5[] = "rw.sys.version.version.version.version flash_offset = *(hi_u32 *)DT_SetGetU32(&g_Element[0], 0)";
    int ret = SetParameter(key5, value5);
    EXPECT_EQ(ret, 0);
    char valueGet[2] = {0};
    char defValue3[] = "value of key > 32 ...";
    ret = GetParameter(key5, defValue3, valueGet, 2);
    EXPECT_EQ(ret, EC_INVALID);
}

HWTEST_F(ParameterTest, parameterTest0010, TestSize.Level0)
{
    char key1[] = "rw.sys.version";
    char value1[] = "10.1.0";
    int ret = SetParameter(key1, value1);
    EXPECT_EQ(ret, 0);
    char valueGet1[32] = {0};
    ret = GetParameter(key1, "version=10.1.0", valueGet1, 32);
    EXPECT_EQ(ret, strlen(valueGet1));

    char key2[] = "rw.product.type";
    char value2[] = "wifi_iot";
    ret = SetParameter(key2, value2);
    EXPECT_EQ(ret, 0);
    char valueGet2[32] = {0};
    ret = GetParameter(key2, "version=10.1.0", valueGet2, 32);
    EXPECT_EQ(ret, strlen(valueGet2));

    char key3[] = "rw.product.manufacturer";
    char value3[] = "HUAWEI";
    ret = SetParameter(key3, value3);
    EXPECT_EQ(ret, 0);
    char valueGet3[32] = {0};
    ret = GetParameter(key3, "version=10.1.0", valueGet3, 32);
    EXPECT_EQ(ret, strlen(valueGet3));

    char key4[] = "rw.product.marketname";
    char value4[] = "Mate 30";
    ret = SetParameter(key4, value4);
    EXPECT_EQ(ret, 0);
    char valueGet4[32] = {0};
    ret = GetParameter(key4, "version=10.1.0", valueGet4, 32);
    EXPECT_EQ(ret, strlen(valueGet4));
}
}  // namespace OHOS