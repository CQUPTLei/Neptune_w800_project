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

#include "gtest/gtest.h"
#include "utils/DMSTestBase.h"
#include "utils/dms_packet.h"
#include "distributed_service_interface.h"

using namespace testing::ext;

static int8_t RunTest(const uint8_t *buffer, uint16_t bufferLen, const TlvParseCallback onTlvParseDone,
                      const StartAbilityCallback onStartAbilityDone)
{
    IDmsFeatureCallback dmsFeatureCallback = {
        .onTlvParseDone = onTlvParseDone,
        .onStartAbilityDone = onStartAbilityDone
    };

    CommuInterInfo interInfo;
    interInfo.payloadLength = bufferLen;
    interInfo.payload = buffer;

    return DmsLiteProcessCommuMsg(&interInfo, &dmsFeatureCallback);
}

class StartAbilityFromRemoteFuncTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        SystemInitProxy();
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
        UninstallHap();
    }
    // Testcase setup
    virtual void SetUp()
    {
    }
    // Testcase teardown
    virtual void TearDown()
    {
    }
};

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0010
 * @tc.name      : User can start ability from remote
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0010, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "AceAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
    ASSERT_EQ(resultCode, DMS_EC_START_ABILITY_SYNC_SUCCESS);
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0020
 * @tc.name      : When start ability from remote with unknown comand id user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0020, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "AceAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    dmsMsgInfo.commandId = 9;

    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
    ASSERT_EQ(resultCode, DMS_EC_UNKNOWN_COMMAND_ID);
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0030
 * @tc.name      : When start ability from remote with unknown bundle name user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0030, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.unknownPackage";
    std::string abilityName = "AceAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };

    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
    ASSERT_EQ(resultCode, DMS_EC_GET_BUNDLEINFO_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0040
 * @tc.name      : When start ability from remote with unknown ability name user can not get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0040, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "unknownAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
    ASSERT_EQ(resultCode, DMS_EC_START_ABILITY_SYNC_SUCCESS);
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0050
 * @tc.name      : When start ability from remote with wrong signature user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0050, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "AceAbility";
    std::string signature = "wrongPublicKey";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
    ASSERT_EQ(resultCode, DMS_EC_CHECK_PERMISSION_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0060
 * @tc.name      : User can start ability repeatedly and no memory leak
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0060, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "AceAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    for (int i = 0; i < PRESSURE_LEVEL0; i++) {
        usleep(OPER_INTERVAL * MS2US);
        int8_t resultCode = RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
        ASSERT_EQ(resultCode, DMS_EC_START_ABILITY_SYNC_SUCCESS);
        printf("[hcpptest]occurs: %d, resultCode:%d \n", i, resultCode);
    }
}

/**
 * @tc.number    : DMSLite_DMS_StartAbilityFromRemoteFunc_0070
 * @tc.name      : When start ability from remote DMS cost time less than 50ms
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(StartAbilityFromRemoteFuncTest, testStartAbilityFromRemoteFunc0070, Function | MediumTest | Level2) {
    std::string bundleName = "com.huawei.helloworld";
    std::string abilityName = "AceAbility";
    std::string signature = "BERxL4Em2dAox98GRbsB31dRcsHDto7hjc5ztnc0cNMsHX33a0B2xgfJIJiGMBiAEcTlW3mbxzm6KIMyrKJj1z8=";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onStartAbilityDone = [] (int8_t errCode) {
        printf("[hcpptest]onStartAbilityDone call and errCode:%d \n", errCode);
    };

    long long totalTime = 0;
    for (int i = 0; i < PRESSURE_LEVEL0; i++) {
        usleep(OPER_INTERVAL * MS2US);

        long long startTime = GetSystemTime();
        RunTest((uint8_t *)buffer, dataSize, nullptr, onStartAbilityDone);
        long long endTime = GetSystemTime();

        totalTime = totalTime + (endTime - startTime);
    }

    printf("[hcpptest]totalTime:%lld, averageTime:%lld \n", totalTime, totalTime / PRESSURE_LEVEL0);

    // usually it costs about 19ms, if bigger than 50ms need to analyse
    ASSERT_EQ((totalTime / PRESSURE_LEVEL0) < 50, TRUE);
}