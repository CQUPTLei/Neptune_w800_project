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

package ohos.junit.distribute;

/**
 * distribute constant
 */
public class DistributeConsts {
    /**
     * default encode format
     */
    public static final String DEFAULT_ENCODING = "UTF-8";

    /**
     * separator for ipAddress
     */
    public static final String DOT_SIGLE = ",";

    /**
     * separator for ipAddress and port
     */
    public static final String COLON_SIGLE = ":";

    /**
     * device ipAddress list
     */
    public static final String IP_CFG_KEY = "agentlist";

    /**
     * device ports
     */
    public static final String PORT_CFG_KEY = "agentport";

    /**
     * empty string
     */
    public static final String EMPTY_STR = "";

    /**
     * start command for testing
     */
    public static final String START_CMD = "start";

    /**
     * stop command for testing
     */
    public static final String STOP_CMD = "stop";

    /**
     * maximum message length
     */
    public static final int MAX_DATA_LENGTH = 1024;

    /**
     * failed code
     */
    public static final int FAILED_CODE = -1;

    /**
     * retry times
     */
    public static final int RETRY_TIME = 3;

    /**
     * time for once waiting
     */
    public static final int WAIT_TIME = 3000;

    /**
     * timeout times
     */
    public static final int TIMEOUT_TIME = 60;

    /**
     * connection timeout between client and server
     */
    public static final int TIME_OUT = WAIT_TIME * TIMEOUT_TIME;

    /**
     * default return value
     */
    public static final String DEFAULT_RESULT = "SUCCESS";

    /**
     * data type enum
     */
    public enum DistributeDataType {
        /**
         * send command from client to server
         */
        DATA_CMD("DATA_CMD"),

        /**
         * send message string
         */
        DATA_MSG("DATA_MSG"),

        /**
         * send notify
         */
        DATA_NOTIFY("DATA_NOTIFY");

        private String value = "";

        /**
         * default type name
         *
         * @param name data type name
         */
        DistributeDataType(String name) {
            this.value = name;
        }

        /**
         * toString
         *
         * @return string
         */
        public String toString() {
            return this.value;
        }
    }

    private DistributeConsts() {
    }
}
