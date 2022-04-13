/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <cstring>
#include <unistd.h>

#include "client_executor/include/i_aie_client.inl"
#include "platform/time/include/time.h"
#include "utils/aie_client_callback.h"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    const int WAIT_CALLBACK_TIME_MS = 2000;
}

class AieClientSyncProcessFunctionTest : public testing::Test {};

/**
 * Constructs ConfigInfo parameters.
 */
static void GetConfigInfo(ConfigInfo &configInfo)
{
    configInfo = {.description = CONFIG_DESCRIPTION};
}

/**
 * Constructs ClientInfo parameters.
 */
static void GetClientInfo(ClientInfo &clientInfo)
{
    const char *str = CLIENT_EXTEND_MSG;
    char *extendMsg = const_cast<char*>(str);
    int len = strlen(str) + 1;

    clientInfo = {
        .clientVersion = CLIENT_VERSION_VALID,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)extendMsg,
    };
}

/**
 * Constructs AlgorithmInfo parameters.
 */
static void GetSyncAlgorithmInfo(AlgorithmInfo &algorithmInfo, bool isAsync, int algorithmType)
{
    const char *str = ALGORITHM_EXTEND_MSG;
    char *extendMsg = const_cast<char*>(str);
    int extendLen = strlen(str) + 1;

    algorithmInfo = {
        .clientVersion = CLIENT_VERSION_VALID,
        .isAsync = isAsync,
        .algorithmType = algorithmType,
        .algorithmVersion = ALGORITHM_VERSION_VALID,
        .isCloud = GetRandomBool(),
        .operateId = GetRandomInt(65535),
        .requestId = GetRandomInt(65535),
        .extendLen = extendLen,
        .extendMsg = (unsigned char*)extendMsg,
    };
}

/**
 * Constructs DataInfo.
 */
static DataInfo GetDataInfo(bool isDataInfoNull = true, const char* dataString = DATA_INFO_DEFAULT)
{
    // Sets default dataInfo to null.
    DataInfo dataInfo = {
        .data = nullptr,
        .length = 0,
    };

    // Sets dataInfo to specified value.
    if (!isDataInfoNull) {
        const char *str = dataString;
        char *data = const_cast<char*>(str);
        int length = strlen(str) + 1;

        dataInfo = {
            .data = reinterpret_cast<unsigned char *>(data),
            .length = length,
        };
    }

    return dataInfo;
}

/**
 * Tests AieClientSyncProcess().
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * isSyncProcessInputInfoNull  Whether the input parameter InputInfo of AieClientSyncProcess is null or not.
 * isSyncProcessSuccess  Whether the expected result of AieClientSyncProcess is successful or not.
 * isExpectedSyncProcessOutputInfoNotNull Whether the expected cb of AieClientSyncProcess is not null or not.
*/
static void TestAieClientSyncProcess(bool isAsync, bool isSyncProcessInputInfoNull, bool isSyncProcessSuccess,
    bool isExpectedSyncProcessOutputInfoNotNull)
{
    // Step 0: Defines variables.
    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    AlgorithmInfo algorithmInfo;
    int algorithmType = isAsync ? ALGORITHM_TYPE_ASYNC : ALGORITHM_TYPE_SYNC;
    GetSyncAlgorithmInfo(algorithmInfo, isAsync, algorithmType);

    ServiceDeadCb *cb = nullptr;
    AIE_NEW(cb, ServiceDeadCb());

    // Step 1: Invokes AieClientInit.
    int initReturnCode = AieClientInit(configInfo, clientInfo, algorithmInfo, cb);
    EXPECT_EQ(RETCODE_SUCCESS, initReturnCode) << "AieClientInit is failed!";
    EXPECT_EQ(true, clientInfo.clientId > 0) << "clientId(" << std::to_string(clientInfo.clientId) << ") is incorrect!";
    EXPECT_EQ(true, clientInfo.sessionId > 0) << "sessionId(" << std::to_string(clientInfo.sessionId)
                                              << ") is incorrect!";

    // Step 2: Invokes AieClientPrepare.
    bool isPrepareInputInfoNull = GetRandomBool();
    DataInfo prepareInputInfo = GetDataInfo(isPrepareInputInfoNull, INPUT_INFO_PREPARE);
    DataInfo prepareOutputInfo = GetDataInfo();

    ClientCallback *callback = nullptr;
    if (isAsync) {
        AIE_NEW(callback, ClientCallback());
    }

    int prepareReturnCode = AieClientPrepare(clientInfo, algorithmInfo, prepareInputInfo, prepareOutputInfo, callback);
    EXPECT_EQ(RETCODE_SUCCESS, prepareReturnCode) << "AieClientPrepare is failed!";
    EXPECT_EQ(true, prepareOutputInfo.data != nullptr) << "Prepare outputInfo is incorrect!";

    // Step 3: Invokes AieClientSyncProcess.
    DataInfo syncProcessInputInfo = GetDataInfo(isSyncProcessInputInfoNull, INPUT_INFO_SYNC_PROCESS);
    DataInfo syncProcessOutputInfo = GetDataInfo();
    int syncProcessReturnCode = AieClientSyncProcess(clientInfo, algorithmInfo, syncProcessInputInfo,
        syncProcessOutputInfo);
    EXPECT_EQ(isSyncProcessSuccess, syncProcessReturnCode == RETCODE_SUCCESS) << "AieClientSyncProcess is failed!";
    EXPECT_EQ(isExpectedSyncProcessOutputInfoNotNull, syncProcessOutputInfo.data != nullptr)
        << "AieClientSyncProcess outputInfo is incorrect!";

    // Step 4: Sleeps.
    StepSleepMs(WAIT_CALLBACK_TIME_MS);

    // Step 5: Invokes AieClientRelease.
    DataInfo releaseInputInfo = GetDataInfo(false, INPUT_INFO_RELEASE);
    int releaseReturnCode =  AieClientRelease(clientInfo, algorithmInfo, releaseInputInfo);
    EXPECT_EQ(RETCODE_SUCCESS, releaseReturnCode) << "AieClientRelease is failed!";

    // Step 6: Invokes AieClientDestroy.
    int destroyReturnCode =  AieClientDestroy(clientInfo);
    EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";

    // Step 7: Deletes callback.
    AIE_DELETE(cb);
    AIE_DELETE(callback);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSyncProcess_01_0100
 * @tc.name   : 01. algorithmInfo中isAsync=false，inputInfo不为空，调用AieClientSyncProcess返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSyncProcessFunctionTest, testAieClientSyncProcessFunction0101, Function | MediumTest | Level2)
{
    HILOGI("[Test]testAieClientSyncProcessFunction0101.");
    TestAieClientSyncProcess(false, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSyncProcess_01_0200
 * @tc.name   : 02 algorithmInfo中isAsync=false，inputInfo为空，调用AieClientSyncProcess返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSyncProcessFunctionTest, testAieClientSyncProcessFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSyncProcessFunction0102.");
    TestAieClientSyncProcess(false, true, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSyncProcess_02_0100
 * @tc.name   : 01. algorithmInfo中isAsync=true，调用AieClientSyncProcess返回失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSyncProcessFunctionTest, testAieClientSyncProcessFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSyncProcessFunction0201.");
    bool isSyncProcessInputInfoNull = GetRandomBool();
    TestAieClientSyncProcess(true, isSyncProcessInputInfoNull, false, false);
}