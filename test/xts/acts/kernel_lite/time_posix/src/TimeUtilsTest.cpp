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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <limits.h>
#include "gtest/gtest.h"
#include "log.h"

using namespace testing::ext;
time_t g_time = 18880;
size_t g_zero = 0;
class TimeUtilsTest : public testing::Test {
};

/**
* @tc.number     SUB_KERNEL_TIME_API_ASCTIME_0100
* @tc.name       test asctime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testAscTime, Function | MediumTest | Level1)
{
    struct tm timeptr = {0};
    timeptr.tm_sec = 0;
    timeptr.tm_min = 10;
    timeptr.tm_hour = 10;
    timeptr.tm_mday = 9;
    timeptr.tm_mon = 7;
    timeptr.tm_year = 120;
    timeptr.tm_wday = 7;

    char * returnStr = asctime(&timeptr);
    LOG("    asctime &timeptr:='{timeptr.tm_year=%d timeptr.tm_mon=%d timeptr.tm_mday=%d}'   "
        "--> returnStr:='%s'\n", timeptr.tm_year, timeptr.tm_mon, timeptr.tm_mday, returnStr);
    EXPECT_STREQ("Sun Aug  9 10:10:00 2020\n", returnStr)
        << "ErrInfo: asctime &timeptr:='{timeptr.tm_year=" << timeptr.tm_year
        << " timeptr.tm_mon=" << timeptr.tm_mon << "' timeptr.tm_mday=" << timeptr.tm_mday
        << "}'   --> returnStr:='" << returnStr << "'";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_ASCTIMER_0100
* @tc.name       test asctimer api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testAscTimeR, Function | MediumTest | Level1)
{
    struct tm timeptr = {0};
    timeptr.tm_sec = 0;
    timeptr.tm_min = 10;
    timeptr.tm_hour = 10;
    timeptr.tm_mday = 9;
    timeptr.tm_mon = 7;
    timeptr.tm_year = 120;
    timeptr.tm_wday = 7;

    char str[26];
    char *returnStr = asctime_r(&timeptr, str);
    EXPECT_STREQ("Sun Aug  9 10:10:00 2020\n", returnStr) << "asctime_r return error!";
    EXPECT_STREQ(returnStr, str) << "asctime_r buf return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_CTIME_0100
* @tc.name       test ctime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testCtime, Function | MediumTest | Level2)
{
    time_t curClock;
    time(&curClock);
    char* returnStr = ctime(&curClock);
    LOG("returnStr = %s\n", returnStr);
    EXPECT_STRNE(returnStr, "") << "ctime return error!";

    returnStr = ctime(&g_time);
    EXPECT_STREQ(returnStr, "Thu Jan  1 05:14:40 1970\n") << "ctime return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_CTIME_R_0100
* @tc.name       test ctime_r api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testCtimeR, Function | MediumTest | Level3)
{
    time_t curClock;
    time(&curClock);
    char str[26];
    char *returnStr = ctime_r(&curClock, str);
    LOG("str = %s", str);
    EXPECT_STRNE(returnStr, "") << "ctime_r return error!";
    EXPECT_STREQ(returnStr, str) << "ctime_r returns not equal";

    returnStr = ctime_r(&g_time, str);
    EXPECT_STREQ(returnStr, "Thu Jan  1 05:14:40 1970\n") << "ctime_r return error!";
    EXPECT_STREQ(returnStr, str) << "ctime_r returns not equal";
}

/**
* @tc.number     SUB_KERNEL_NDKAPI_TIME_DIFFTIME_0100
* @tc.name       test difftime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testDifftime, Function | MediumTest | Level2)
{
    time_t timeStart, timeEnd;
    double returnVal;
    time(&timeStart);
    sleep(1);
    time(&timeEnd);
    returnVal = difftime(timeEnd, timeStart);
    LOG("    difftime timeEnd:='%lld' timeStart:='%lld'   "
        "--> returnVal:='%f'\n", timeEnd, timeStart, returnVal);
    LOG("    sizeof timeEnd:='%d'   sizeof timeStart:='%d'\n", sizeof(timeEnd), sizeof(timeStart));
    EXPECT_GE(returnVal, 0)
        << "ErrInfo: difftime timeEnd:='" << timeEnd << "' timeStart:='"
        << timeStart << "'   --> returnVal:='" << returnVal << "'";
    EXPECT_LE(returnVal, 2)
        << "ErrInfo: difftime timeEnd:='" << timeEnd << "' timeStart:='"
        << timeStart << "'   --> returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_NDKAPI_TIME_TIMEGM_0100
* @tc.name       test timegm api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testTimegm, Function | MediumTest | Level3)
{
    struct tm timeptr = {0};
    time_t timeThis;
    timeptr.tm_sec = 0;
    timeptr.tm_min = 10;
    timeptr.tm_hour = 10;
    timeptr.tm_mday = 9;
    timeptr.tm_mon = 7;
    timeptr.tm_year = 120;
    timeptr.tm_wday = 5;

    timeThis = timegm(&timeptr);
    LOG("    timegm &timeptr:='{timeptr.tm_year=%d timeptr.tm_mon=%d timeptr.tm_mday=%d}'   "
        "--> return timeThis:='%lld'\n", timeptr.tm_year, timeptr.tm_mon, timeptr.tm_mday, timeThis);
    EXPECT_GE(timeThis, 1)
        << "ErrInfo: timegm &timeptr:='{timeptr.tm_year=" << timeptr.tm_year
        << " timeptr.tm_mon=" << timeptr.tm_mon << " timeptr.tm_mday="
        << timeptr.tm_mday << "}'   --> return timeThis:='" << timeThis << "'";

    struct tm *stm2 = gmtime(&g_time);
    time_t timep = timegm(stm2);
    LOG("stm = %s;mktime:%ld\n", asctime(stm2), (long)timep);
    EXPECT_EQ(timep, g_time) << "timegm return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_GMTIME_0100
* @tc.name       test gmtime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testGmtime, Function | MediumTest | Level3)
{
    time_t time1 = 18880;
    struct tm *stm = gmtime(&time1);
    EXPECT_EQ(stm->tm_hour, 05) << "gmtime return error!";
    EXPECT_STREQ(asctime(stm), "Thu Jan  1 05:14:40 1970\n") << "gmtime return error!";

    time1 = LONG_MAX;
    stm = gmtime(&time1);
    EXPECT_STREQ(asctime(stm), "Tue Jan 19 03:14:07 2038\n") << "gmtime return error!";

    time1 = 253402300799;
    stm = gmtime(&time1);
    EXPECT_STREQ(asctime(stm), "Fri Dec 31 23:59:59 9999\n") << "gmtime return error!";

    time1 = LONG_MIN;
    stm = gmtime(&time1);
    EXPECT_STREQ(asctime(stm), "Fri Dec 13 20:45:52 1901\n") << "gmtime return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_GMTIMER_0100
* @tc.name       test gmtime_r api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testGmtimeR, Function | MediumTest | Level3)
{
    struct tm res = {0};
    struct tm *stm = gmtime_r(&g_time, &res);
    EXPECT_EQ(stm->tm_hour, 05) << "gmtime_r return error!";
    EXPECT_STREQ(asctime(stm), "Thu Jan  1 05:14:40 1970\n") << "gmtime_r return error!";
    EXPECT_TRUE(stm == &res) << "gmtime_r returns not equal";

    time_t timeNow;
    time(&timeNow);
    stm = gmtime_r(&timeNow, &res);
    EXPECT_EQ(stm->tm_year, 70) << "gmtime_r return error!";
    EXPECT_STRNE(asctime(stm), "") << "gmtime_r return error!";
    EXPECT_TRUE(stm == &res) << "gmtime_r returns not equal";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_MKTIME_0100
* @tc.name       test mktime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testMktime, Function | MediumTest | Level2)
{
    struct tm timeptr = {0};
    timeptr.tm_sec = 0;
    timeptr.tm_min = 10;
    timeptr.tm_hour = 10;
    timeptr.tm_mday = 9;
    timeptr.tm_mon = 7;
    timeptr.tm_year = 120;
    timeptr.tm_wday = 7;
    EXPECT_EQ(mktime(&timeptr), 1596967800) << "mktime return error!";

    time_t timep = mktime(localtime(&g_time));
    EXPECT_EQ(timep, 18880) << "mktime return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_STRFTIME_0100
* @tc.name       test strftime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testStrftime, Function | MediumTest | Level3)
{
    char buffer[32];
    time_t mtime = 18880;
    size_t ftime = strftime(buffer, 80, "%Ex %EX %A", localtime(&mtime));
    EXPECT_GT(ftime, g_zero) << "strftime return error!";
    EXPECT_STREQ(buffer, "01/01/70 05:14:40 Thursday") << "buffer return error!";

    mtime = LONG_MAX;
    ftime = strftime(buffer, 80, "%y-%m-%d %H:%M:%S", localtime(&mtime));
    EXPECT_STREQ(buffer, "38-01-19 03:14:07") << "buffer return error!";

    mtime = 253402300799;
    ftime = strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&mtime));
    EXPECT_STREQ(buffer, "9999-12-31 23:59:59") << "buffer return error!";

    mtime = LONG_MIN;
    ftime = strftime(buffer, 80, "%x %X", localtime(&mtime));
    EXPECT_STREQ(buffer, "12/13/01 20:45:52") << "buffer return error!";

    ftime = strftime(buffer, 11, "%Y-%m-%d %H:%M:%S", localtime(&mtime));
    EXPECT_EQ(ftime, g_zero) << "strftime return error!";
    EXPECT_STREQ(buffer, "1901-12-13") << "buffer return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_STRFTIMEL_0100
* @tc.name       test strftime_l api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testStrftimeL, Function | MediumTest | Level2)
{
    struct tm *tm1;
    char buffer[32];

    tm1 = localtime(&g_time);
    size_t ftime = strftime_l(buffer, 80, "%F %T %Z", tm1, (locale_t)0);
    EXPECT_GT(ftime, g_zero) << "strftime return error!";
    EXPECT_STREQ(buffer, "1970-01-01 05:14:40 UTC") << "buffer return error!";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_WCSFTIME_0100
* @tc.name       test wcsftime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(TimeUtilsTest, testWcsftime, Function | MediumTest | Level2)
{
    wchar_t buff[48];
    size_t len = wcsftime(buff, sizeof(buff), L"%A %c", localtime(&g_time));
    LOG("buff = %ls, len = %ld\n", buff, (long)len);
    EXPECT_EQ(len, (size_t)33) << "wcsftime return error!";
    EXPECT_STREQ(buff, L"Thursday Thu Jan  1 05:14:40 1970") << "buff return error!";

    len = wcsftime(buff, 9, L"%A %c", localtime(&g_time));
    LOG("buff = %ls, len = %ld\n", buff, (long)len);
    EXPECT_EQ(len, g_zero) << "wcsftime return error!";
    EXPECT_STREQ(buff, L"Thursday") << "buff return error!";
}