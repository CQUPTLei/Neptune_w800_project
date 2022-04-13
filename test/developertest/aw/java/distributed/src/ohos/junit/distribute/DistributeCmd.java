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

import ohos.junit.distribute.DistributeConsts.DistributeDataType;

import java.io.Serializable;

/**
 * command structure
 */
public class DistributeCmd implements Serializable {
    /**
     * serial num
     */
    private static final long serialVersionUID = 1L;

    /**
     * command type
     */
    private DistributeDataType cmdType = DistributeDataType.DATA_NOTIFY;

    /**
     * command between client and server
     */
    private String command = "";

    /**
     * command params
     */
    private String param = "";

    /**
     * expect return value
     */
    private String expectValue = "";

    /**
     * create distribute command
     *
     * @param distributeDataType command type
     * @param command command
     */
    public DistributeCmd(DistributeDataType distributeDataType, String command) {
        this.cmdType = distributeDataType;
        this.command = command;
    }

    /**
     * create distribute command
     *
     * @param cmdType command type
     * @param command command
     * @param param param
     * @param expectValue return value
     */
    public DistributeCmd(DistributeDataType cmdType, String command, String param, String expectValue) {
        this.cmdType = cmdType;
        this.command = command;
        this.param = param;
        this.expectValue = expectValue;
    }

    public DistributeDataType getCmdType() {
        return cmdType;
    }

    public void setCmdType(DistributeDataType cmdType) {
        this.cmdType = cmdType;
    }

    public String getCommand() {
        return command;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public String getParam() {
        return param;
    }

    public void setParam(String param) {
        this.param = param;
    }

    public String getExpectValue() {
        return expectValue;
    }

    public void setExpectValue(String expectValue) {
        this.expectValue = expectValue;
    }

    @Override
    public String toString() {
        return "DistributeCmd{cmdType=" + cmdType + ", command=" + command + ", param=" + param + ", expectValue="
            + expectValue + "}";
    }
}