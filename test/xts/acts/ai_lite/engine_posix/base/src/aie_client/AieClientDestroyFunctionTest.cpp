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
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_client_callback.h"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

class AieClientDestroyFunctionTest : public testing::Test {};

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
 * Tests AieClientDestroy. Invoke sequence: AieClientInit-AieClientPrepare-AieClientSyncProcess/AieClientAsyncProcess
 * -AieClientRelease-AieClientDestroy.
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 */
static void TestAieClientDestroy(bool isAsync)
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

    IClientCb *callback = nullptr;
    if (isAsync) {
        AIE_NEW(callback, ClientCallback());
    }
    int prepareReturnCode = AieClientPrepare(clientInfo, algorithmInfo, prepareInputInfo, prepareOutputInfo, callback);
    EXPECT_EQ(RETCODE_SUCCESS, prepareReturnCode) << "AieClientPrepare is failed!";
    EXPECT_EQ(true, prepareOutputInfo.data != nullptr) << "Prepare outputInfo is incorrect!";

    // Step 3: Invokes AieClientSyncProcess/AieClientAsyncProcess.
    bool isProcessInputInfoNull = GetRandomBool();
    if (isAsync) {
        DataInfo asyncProcessInputInfo = GetDataInfo(isProcessInputInfoNull, INPUT_INFO_ASYNC_PROCESS);
        int asyncProcessReturnCode = AieClientAsyncProcess(clientInfo, algorithmInfo, asyncProcessInputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, asyncProcessReturnCode) << "AieClientAsyncProcess is failed!";
    } else {
        DataInfo syncProcessInputInfo = GetDataInfo(isProcessInputInfoNull, INPUT_INFO_SYNC_PROCESS);
        DataInfo syncProcessOutputInfo = GetDataInfo();
        int syncProcessReturnCode = AieClientSyncProcess(clientInfo, algorithmInfo, syncProcessInputInfo,
                                                         syncProcessOutputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, syncProcessReturnCode) << "AieClientSyncProcess is failed!";
        EXPECT_EQ(true, syncProcessOutputInfo.data != nullptr) << "AieClientSyncProcess outputInfo is incorrect!";
    }

    // Step 4: Invokes AieClientRelease.
    bool isReleaseInputInfoNull = GetRandomBool();
    DataInfo releaseInputInfo = GetDataInfo(isReleaseInputInfoNull, INPUT_INFO_RELEASE);
    int releaseReturnCode = AieClientRelease(clientInfo, algorithmInfo, releaseInputInfo);
    EXPECT_EQ(RETCODE_SUCCESS, releaseReturnCode) << "AieClientRelease is failed!";

    // Step 5: Invokes AieClientDestroy.
    int destroyReturnCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";

    // Step 6: Deletes callback.
    AIE_DELETE(cb);
    if (isAsync) {
        AIE_DELETE(callback);
    }
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientDestroy_0100
 * @tc.name   : 01. 初始化之后，调用AieClientDestroy返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientDestroyFunctionTest, testAieClientDestroyFunction001, Function | MediumTest | Level2)
{
    HILOGI("[Test]testAieClientDestroyFunction001.");
    bool isAsync = GetRandomBool();
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
    AIE_DELETE(cb);

    // Step 2: Invokes AieClientDestroy.
    int destroyReturnCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientDestroy_0200
 * @tc.name   : 02. 同步执行算法之后，调用AieClientDestroy返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientDestroyFunctionTest, testAieClientDestroyFunction002, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientDestroyFunction002.");
    TestAieClientDestroy(false);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientDestroy_0300
 * @tc.name   : 03. 异步执行算法之后，调用AieClientDestroy返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientDestroyFunctionTest, testAieClientDestroyFunction003, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientDestroyFunction003.");
    TestAieClientDestroy(true);
}