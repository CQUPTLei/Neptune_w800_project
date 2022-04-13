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

#ifndef HOS_LITE_HIVIEW_COMMAND_H
#define HOS_LITE_HIVIEW_COMMAND_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

void HilogCmdProc(const char *cmd);
void HieventCmdProc(const char *cmd);
void DumpCmdProc(const char *cmd);
/**
 * Dynamically adjust the Log Output Level.
 * @param level log level. Logs of this level or higher will be generated.
 * @return TRUE/FALSE
 **/
boolean SetLogLevel(uint8 level);

/**
 * Enable or disable the log function.
 * @param flag HIVIEW_FEATURE_ON/HIVIEW_FEATURE_OFF
 **/
void SwitchLog(uint8 flag);

/**
 * Enable the log output of a specified module.
 * @param mod module id.
 * @return TRUE/FALSE
 **/
boolean OpenLogOutputModule(uint8 mod);

/**
 * Disable the log output of a specified module.
 * @param mod module id.
 * @return TRUE/FALSE
 **/
boolean CloseLogOutputModule(uint8 mod);

/**
 * Enable the log output of a specified module and disable the log output of other modules.
 * @param mod module id. If the value of the module id is HILOG_MODULE_MAX, the log output of all modules is enabled.
 * @return TRUE/FALSE
 **/
boolean SetLogOutputModule(uint8 mod);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_COMMAND_H */
