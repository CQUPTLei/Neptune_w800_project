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

#ifndef AIE_CLIENT_PERF_BASELINE_H
#define AIE_CLIENT_PERF_BASELINE_H

#include <iostream>

namespace AI_ENGINE_TEST {
/**
 *  Configures which type of device to be test.
 */
const int DEVICE_TYPE_L1 = 1;
const int DEVICE_TYPE_L2 = 2;
const int DEVICE_TYPE = DEVICE_TYPE_L1;

/**
 *  PerfBaseline to test.
 */
struct PerfBaseline {
    int T01; // us, AieClientInit
    int T02; // us, AieClientPrepare
    int T03; // us, AieClientASyncProcess
    int T04; // us, AieClientSyncProcess
    int T05; // us, AieClientRelease
    int T06; // us, AieClientDestroy
    int T07; // us, AieClientSetOption
    int T08; // us, AieClientGetOption
};

const int PERF_BASELINE_L1_AIE_CLIENT_INIT = 100000;
const int PERF_BASELINE_L1_AIE_CLIENT_PREPARE = 300000;
const int PERF_BASELINE_L1_AIE_CLIENT_ASYNC_PROCESS = 150000;
const int PERF_BASELINE_L1_AIE_CLIENT_SYNC_PROCESS = 100000;
const int PERF_BASELINE_L1_AIE_CLIENT_RELEASE = 300000;
const int PERF_BASELINE_L1_AIE_CLIENT_DESTROY = 100000;
const int PERF_BASELINE_L1_AIE_CLIENT_SET_OPTION = 100000;
const int PERF_BASELINE_L1_AIE_CLIENT_GET_OPTION = 100000;

PerfBaseline L1 = {
    .T01 = PERF_BASELINE_L1_AIE_CLIENT_INIT, // us, AieClientInit
    .T02 = PERF_BASELINE_L1_AIE_CLIENT_PREPARE, // us, AieClientPrepare
    .T03 = PERF_BASELINE_L1_AIE_CLIENT_ASYNC_PROCESS, // us, AieClientASyncProcess
    .T04 = PERF_BASELINE_L1_AIE_CLIENT_SYNC_PROCESS, // us, AieClientSyncProcess
    .T05 = PERF_BASELINE_L1_AIE_CLIENT_RELEASE, // us, AieClientRelease
    .T06 = PERF_BASELINE_L1_AIE_CLIENT_DESTROY, // us, AieClientDestroy
    .T07 = PERF_BASELINE_L1_AIE_CLIENT_SET_OPTION, // us, AieClientSetOption
    .T08 = PERF_BASELINE_L1_AIE_CLIENT_GET_OPTION, // us, AieClientGetOption
};

const int PERF_BASELINE_L2_AIE_CLIENT_INIT = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_PREPARE = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_ASYNC_PROCESS = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_SYNC_PROCESS = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_RELEASE = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_DESTROY = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_SET_OPTION = 500;
const int PERF_BASELINE_L2_AIE_CLIENT_GET_OPTION = 500;

PerfBaseline L2 = {
    .T01 = PERF_BASELINE_L2_AIE_CLIENT_INIT, // us, AieClientInit
    .T02 = PERF_BASELINE_L2_AIE_CLIENT_PREPARE, // us, AieClientPrepare
    .T03 = PERF_BASELINE_L2_AIE_CLIENT_ASYNC_PROCESS, // us, AieClientASyncProcess
    .T04 = PERF_BASELINE_L2_AIE_CLIENT_SYNC_PROCESS, // us, AieClientSyncProcess
    .T05 = PERF_BASELINE_L2_AIE_CLIENT_RELEASE, // us, AieClientRelease
    .T06 = PERF_BASELINE_L2_AIE_CLIENT_DESTROY, // us, AieClientDestroy
    .T07 = PERF_BASELINE_L2_AIE_CLIENT_SET_OPTION, // us, AieClientSetOption
    .T08 = PERF_BASELINE_L2_AIE_CLIENT_GET_OPTION, // us, AieClientGetOption
};

/**
 * Gets Performance BaseLine according to deviceType.
 */
PerfBaseline GetPerformanceBaseLine(int deviceType)
{
    PerfBaseline aieClientPerfBase;
    switch (deviceType) {
        case DEVICE_TYPE_L1: {
            aieClientPerfBase = L1;
            break;
        }
        case DEVICE_TYPE_L2: {
            aieClientPerfBase = L2;
            break;
        }
        default: {
            aieClientPerfBase = L1;
            break;
        }
    }
    return aieClientPerfBase;
}
}
#endif // AIE_CLIENT_PERF_BASELINE_H