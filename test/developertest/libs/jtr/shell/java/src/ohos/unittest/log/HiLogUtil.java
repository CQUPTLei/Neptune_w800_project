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

package ohos.unittest.log;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;

import java.util.Locale;

/**
 * HiLog Util
 */
public class HiLogUtil {
    /**
     * Log tag
     */
    private static final String TAG_LOG = "AppUnitTest";

    /**
     * HiLogLabel instance
     */
    private static final HiLogLabel LABEL_LOG = new HiLogLabel(3, 0, HiLogUtil.TAG_LOG);

    /**
     * Log format
     */
    private static final String LOG_FORMAT = " %s: %s";

    /**
     * LogUtil Constructor
     */
    private HiLogUtil() {
    }

    /**
     * Print debug log
     *
     * @param classType class name
     * @param msg log message
     */
    @SuppressWarnings("rawtypes")
    public static void debug(Class classType, String msg) {
        HiLog.debug(LABEL_LOG, String.format(Locale.ROOT, LOG_FORMAT, classType.getSimpleName(), msg));
    }

    /**
     * Print info log
     *
     * @param classType class name
     * @param msg log message
     */
    @SuppressWarnings("rawtypes")
    public static void info(Class classType, String msg) {
        HiLog.info(LABEL_LOG, String.format(Locale.ROOT, LOG_FORMAT, classType.getSimpleName(), msg));
    }

    /**
     * Print info log
     *
     * @param className class name
     * @param msg log message
     */
    public static void info(String className, String msg) {
        HiLog.info(LABEL_LOG, String.format(Locale.ROOT, LOG_FORMAT, className, msg));
    }

    /**
     * Print error log
     *
     * @param classType class name
     * @param msg log message
     */
    @SuppressWarnings("rawtypes")
    public static void error(Class classType, String msg) {
        HiLog.error(LABEL_LOG, String.format(Locale.ROOT, LOG_FORMAT, classType.getSimpleName(), msg));
    }

    /**
     * Print error log
     *
     * @param className class name
     * @param msg log message
     */
    public static void error(String className, String msg) {
        HiLog.error(LABEL_LOG, String.format(Locale.ROOT, LOG_FORMAT, className, msg));
    }

    /**
     * print String info to log
     *
     * @param classType class name
     * @param msg log message
     */
    @SuppressWarnings("rawtypes")
    public static void printInfo(Class classType, String msg) {
        HiLog.info(LABEL_LOG, "classType is %s, message is %s", classType, msg);
    }

    /**
     * print String error to log
     *
     * @param classType class name
     * @param msg log message
     */
    @SuppressWarnings("rawtypes")
    public static void printError(Class classType, String msg) {
        HiLog.error(LABEL_LOG, "classType is %s, message is %s", classType, msg);
    }
}
