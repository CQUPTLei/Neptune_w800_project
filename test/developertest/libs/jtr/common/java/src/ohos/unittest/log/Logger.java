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

package ohos.unittest.log;

/**
 * Logger
 */
public final class Logger {
    /**
     * LogFile instance
     */
    private static LogFile logFile = LogFile.getInstance();

    /**
     * Logger Constructor
     */
    private Logger() {
    }

    /**
     * createLogFile
     *
     * @apiNote createLogFile
     * @param filePath Log File path
     */
    public static void createLogFile(String filePath) {
        if (logFile != null) {
            logFile.createLogFile(filePath);
        }
    }

    /**
     * Record log information
     *
     * @apiNote info
     * @param content log information
     */
    public static void info(String content) {
        System.out.println(content);
        if (logFile != null) {
            logFile.println(content);
        }
    }
}
