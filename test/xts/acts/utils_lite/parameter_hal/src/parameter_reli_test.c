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

#include "ohos_types.h"
#include <securec.h>
#include "hctest.h"
#include "parameter.h"
#include "parameter_utils.h"

#define QUERY_TIMES    50

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is parameter
 * @param        : test suit name is ParameterReliTestSuite
 */
LITE_TEST_SUIT(utils, parameter, ParameterReliTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL ParameterReliTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL ParameterReliTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_PARAMETER_5800
 * @tc.name      : Obtaining ProductType for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli001, Function | MediumTest | Level1)
{
    char* value1 = GetProductType();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetProductType();
    free(value);
    }
    char* value2 = GetProductType();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_5900
 * @tc.name      : Obtaining Manufacture for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli002, Function | MediumTest | Level1)
{
    char* value1 = GetManufacture();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetManufacture();
    free(value);
    }
    char* value2 = GetManufacture();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6000
 * @tc.name      : Obtaining Brand for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli003, Function | MediumTest | Level1)
{
    char* value1 = GetBrand();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBrand();
    free(value);
    }
    char* value2 = GetBrand();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6100
 * @tc.name      : Obtaining MarketName for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli004, Function | MediumTest | Level1)
{
    char* value1 = GetMarketName();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetMarketName();
    free(value);
    }
    char* value2 = GetMarketName();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6200
 * @tc.name      : Obtaining ProductSeries for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli005, Function | MediumTest | Level1)
{
    char* value1 = GetProductSeries();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetProductSeries();
    free(value);
    }
    char* value2 = GetProductSeries();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6300
 * @tc.name      : Obtaining ProductModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli006, Function | MediumTest | Level1)
{
    char* value1 = GetProductModel();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetProductModel();
    free(value);
    }
    char* value2 = GetProductModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6400
 * @tc.name      : Obtaining HardwareModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli007, Function | MediumTest | Level1)
{
    char* value1 = GetHardwareModel();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetHardwareModel();
    free(value);
    }
    char* value2 = GetHardwareModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6500
 * @tc.name      : Obtaining HardwareProfile for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli008, Function | MediumTest | Level1)
{
    char* value1 = GetHardwareProfile();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetHardwareProfile();
    free(value);
    }
    char* value2 = GetHardwareProfile();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6600
 * @tc.name      : Obtaining Serial for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli009, Function | MediumTest | Level1)
{
    char* value1 = GetSerial();
    if (value1 == NULL) {
        printf("The serial number needs to be written\n");
        TEST_IGNORE();
    }
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetSerial();
    free(value);
    }
    char* value2 = GetSerial();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6700
 * @tc.name      : Obtaining OsName for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli010, Function | MediumTest | Level1)
{
    char* value1 = GetOsName();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetOsName();
    free(value);
    }
    char* value2 = GetOsName();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6800
 * @tc.name      : Obtaining DisplayVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli011, Function | MediumTest | Level1)
{
    char* value1 = GetDisplayVersion();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetDisplayVersion();
    free(value);
    }
    char* value2 = GetDisplayVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6900
 * @tc.name      : Obtaining BootloaderVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli012, Function | MediumTest | Level1)
{
    char* value1 = GetBootloaderVersion();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBootloaderVersion();
    free(value);
    }
    char* value2 = GetBootloaderVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7000
 * @tc.name      : Obtaining SecurityPatchTag for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli013, Function | MediumTest | Level1)
{
    char* value1 = GetSecurityPatchTag();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetSecurityPatchTag();
    free(value);
    }
    char* value2 = GetSecurityPatchTag();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7100
 * @tc.name      : Obtaining AbiList for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli014, Function | MediumTest | Level1)
{
    char* value1 = GetAbiList();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetAbiList();
    free(value);
    }
    char* value2 = GetAbiList();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7200
 * @tc.name      : Obtaining FirstApiLevel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli015, Function | MediumTest | Level1)
{
    char* value1 = GetFirstApiLevel();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetFirstApiLevel();
    free(value);
    }
    char* value2 = GetFirstApiLevel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7300
 * @tc.name      : Obtaining IncrementalVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli016, Function | MediumTest | Level1)
{
    char* value1 = GetIncrementalVersion();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetIncrementalVersion();
    free(value);
    }
    char* value2 = GetIncrementalVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7400
 * @tc.name      : Obtaining VersionId for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli017, Function | MediumTest | Level1)
{
    char* value1 = GetVersionId();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetVersionId();
    free(value);
    }
    char* value2 = GetVersionId();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7500
 * @tc.name      : Obtaining BuildType for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli018, Function | MediumTest | Level1)
{
    char* value1 = GetBuildType();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBuildType();
    free(value);
    }
    char* value2 = GetBuildType();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7600
 * @tc.name      : Obtaining BuildUser for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli019, Function | MediumTest | Level1)
{
    char* value1 = GetBuildUser();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBuildUser();
    free(value);
    }
    char* value2 = GetBuildUser();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7700
 * @tc.name      : Obtaining BuildHost for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli020, Function | MediumTest | Level1)
{
    char* value1 = GetBuildHost();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBuildHost();
    free(value);
    }
    char* value2 = GetBuildHost();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7800
 * @tc.name      : Obtaining BuildTime for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli021, Function | MediumTest | Level1)
{
    char* value1 = GetBuildTime();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBuildTime();
    free(value);
    }
    char* value2 = GetBuildTime();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7900
 * @tc.name      : Obtaining BuildRootHash for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli022, Function | MediumTest | Level1)
{
    char* value1 = GetBuildRootHash();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetBuildRootHash();
    free(value);
    }
    char* value2 = GetBuildRootHash();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_8000
 * @tc.name      : Obtaining SoftwareModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli023, Function | MediumTest | Level1)
{
    char* value1 = GetSoftwareModel();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetSoftwareModel();
    free(value);
    }
    char* value2 = GetSoftwareModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_8100
 * @tc.name      : Obtaining SdkApiLevel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli024, Function | MediumTest | Level1)
{
    char* value1 = GetSdkApiLevel();
    IsEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    char* value = GetSdkApiLevel();
    free(value);
    }
    char* value2 = GetSdkApiLevel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
    free(value1);
    free(value2);
};

RUN_TEST_SUITE(ParameterReliTestSuite);
