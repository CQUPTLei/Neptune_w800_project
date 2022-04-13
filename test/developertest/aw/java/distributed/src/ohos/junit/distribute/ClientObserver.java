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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * client for distribute test framework
 */
public class ClientObserver implements ISocketObserver {
    private static final String LOG_CLIENT_NAME = "ClientObserver";
    private Socket clientSocket = null;

    private PrintWriter printWriter = null;

    private BufferedReader bufferedReader = null;

    private boolean isConnect = false;

    private String ipAddress = DistributeConsts.EMPTY_STR;

    private int port = 0;

    private boolean isStart = false;

    private int tryTime = 0;

    /**
     * construct client
     *
     * @param ipAddress ip
     * @param port port
     */
    public ClientObserver(String ipAddress, int port) {
        this.ipAddress = ipAddress;
        this.port = port;
    }

    /**
     * send message
     *
     * @apiNote send message
     * @param message message
     * @param expectValue expected value
     * @return result code
     */
    public int sendMessage(String message, String expectValue) {
        if ((message.length() + expectValue.length()) < DistributeConsts.MAX_DATA_LENGTH) {
            return sendMsgToAgent(
                new DistributeCmd(DistributeConsts.DistributeDataType.DATA_MSG, message, "", expectValue), null);
        }
        return DistributeConsts.FAILED_CODE;
    }

    /**
     * send message contains callback
     *
     * @apiNote send message contains callback
     *
     * @param message message
     * @param expectValue expected value
     * @param callback callback
     * @return result code
     */
    public int sendMessage(String message, String expectValue, ICallBack callback) {
        if ((message.length() + expectValue.length()) < DistributeConsts.MAX_DATA_LENGTH) {
            return sendMsgToAgent(
                new DistributeCmd(DistributeConsts.DistributeDataType.DATA_MSG, message, "", expectValue), callback);
        }
        return DistributeConsts.FAILED_CODE;
    }

    /**
     * send command to agent
     *
     * @apiNote send command to agent
     *
     * @param command command
     * @param cmdParam param
     * @param expectValue expected value
     * @return result code
     */
    public int runCommandOnAgent(String command, String cmdParam, String expectValue) {
        return sendMsgToAgent(
            new DistributeCmd(DistributeConsts.DistributeDataType.DATA_CMD, command, cmdParam, expectValue), null);
    }

    /**
     * send command to agent contains callback
     *
     * @apiNote send command to agent contains callback
     *
     * @param command command
     * @param cmdParam param
     * @param expectValue expected value
     * @param callback callback
     * @return result code
     */
    public int runCommandOnAgent(String command, String cmdParam, String expectValue, ICallBack callback) {
        return sendMsgToAgent(
            new DistributeCmd(DistributeConsts.DistributeDataType.DATA_CMD, command, cmdParam, expectValue), callback);
    }

    private int sendMsgToAgent(DistributeCmd formatCmd, ICallBack callback) {
        if (formatCmd != null) {
            try {
                this.printWriter.println(CommonUtil.getInstance().serializeToStr(formatCmd));
                this.printWriter.flush();
            } catch (IOException e) {
                CommonUtil.getInstance().info(LOG_CLIENT_NAME,
                    "sendMsgToAgent serializeToStr failed." + e.getMessage());
                return DistributeConsts.FAILED_CODE;
            }

            Object readObj = null;
            String info = null;
            try {
                info = bufferedReader.readLine();
                while (this.isConnect && info != null) {
                    readObj = CommonUtil.getInstance().deserializeToObj(info);
                    if (readObj instanceof DistributeCmd) {
                        DistributeCmd result = (DistributeCmd) readObj;
                        return commonHandleCmd(formatCmd, result, callback);
                    }
                }
            } catch (IOException e) {
                CommonUtil.getInstance().info(LOG_CLIENT_NAME, "sendMsgToAgent IOException." + e.getMessage());
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    private int commonHandleCmd(DistributeCmd sourceCmd, DistributeCmd returnCmd, ICallBack callback) {
        if (callback != null) {
            return callback.processDistCmd(returnCmd);
        }
        if (returnCmd == null) {
            return DistributeConsts.FAILED_CODE;
        }
        if (!returnCmd.getExpectValue().isEmpty()) {
            if (sourceCmd.getExpectValue().equals(returnCmd.getExpectValue())) {
                return 0;
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    @Override
    public boolean initEnvironment() {
        try {
            this.clientSocket = new Socket(this.ipAddress, this.port);
            bufferedReader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            this.printWriter = new PrintWriter(new OutputStreamWriter(this.clientSocket.getOutputStream()));
        } catch (UnknownHostException e) {
            CommonUtil.getInstance().info(LOG_CLIENT_NAME, "client socket UnknownHostException." + e.getMessage());
            return false;
        } catch (IOException e) {
            CommonUtil.getInstance().info(LOG_CLIENT_NAME, "client socket IOException." + e.getMessage());
            return false;
        }
        this.isConnect = true;
        return true;
    }

    @Override
    public void updateSocket(String command) {
        switch (String.valueOf(command)) {
            case DistributeConsts.START_CMD:
                startCommand();
                break;
            case DistributeConsts.STOP_CMD:
                releaseEnvironment();
                break;
            default:
                CommonUtil.getInstance().info(LOG_CLIENT_NAME, "command not found.");
        }
    }

    private void startCommand() {
        if (!isStart) {
            isStart = true;
            while (!initEnvironment()) {
                if (this.tryTime < DistributeConsts.RETRY_TIME) {
                    try {
                        Thread.sleep(DistributeConsts.WAIT_TIME);
                    } catch (InterruptedException e1) {
                        CommonUtil.getInstance().info(LOG_CLIENT_NAME,
                                "client socket sleep InterruptedException." + e1.getMessage());
                    }
                    this.tryTime++;
                } else {
                    break;
                }
            }
        }
    }

    @Override
    public void releaseEnvironment() {
        sendMsgToAgent(new DistributeCmd(DistributeDataType.DATA_NOTIFY, DistributeConsts.STOP_CMD), null);
        if (this.clientSocket != null) {
            try {
                this.clientSocket.close();
            } catch (IOException e) {
                CommonUtil.getInstance().info(LOG_CLIENT_NAME, "close client socket failed." + e.getMessage());
            }
        }
        if (this.bufferedReader != null) {
            try {
                this.bufferedReader.close();
            } catch (IOException e) {
                CommonUtil.getInstance().info(LOG_CLIENT_NAME, "close client outputStream failed." + e.getMessage());
            }
        }
        if (this.printWriter != null) {
            this.printWriter.close();
        }
    }

    public boolean isConnect() {
        return isConnect;
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public void setIpAddress(String ipAddress) {
        this.ipAddress = ipAddress;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public boolean isStart() {
        return isStart;
    }

    public void setStart(boolean isStarted) {
        this.isStart = isStarted;
    }

    public int getTryTime() {
        return tryTime;
    }

    public void setTryTime(int tryTime) {
        this.tryTime = tryTime;
    }

    public void setConnect(boolean isConnected) {
        this.isConnect = isConnected;
    }
}