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

import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

import java.util.Timer;
import java.util.TimerTask;


/**
 * distribute test agent
 */
public abstract class DistributeAgent {
    private static final String LOG_AGENT_NAME = "DistributeAgent";

    /**
     * default construct
     */
    public DistributeAgent() {
    }

    /**
     * test agent initialization, implemented by subclasses
     */
    public abstract void setUpDistribute();

    /**
     * test agent environment cleanup, implemented by subclasses
     */
    public abstract void tearDownDistribute();

    /**
     * process command from major device to agent device, implemented by subclasses
     *
     * @param command command to send agent device
     * @param cmdParam command param
     * @param expectValue expect return value
     * @return DistributeCmd object after processing the command
     */
    public abstract DistributeCmd onProcessCmd(String command, String cmdParam,
        String expectValue);

    /**
     * process message from major device to agent device, implemented by subclasses
     *
     * @param message command to send agent device
     * @return DistributeCmd object after processing the command
     */
    public abstract DistributeCmd onProcessMsg(String message);

    /**
     * start distribute test agent
     *
     * @apiNote start distribute test agent
     *
     * @return success-true,failed-false
     */
    public boolean startDistributeAgent() {
        DistributeCfg distributeCfg = new DistributeCfg();
        distributeCfg.readDistributeCfg("agent.desc");

        if (distributeCfg != null) {
            for (String ipAddress : distributeCfg.getIpList()) {
                SocketServer socketServer = new SocketServer(ipAddress,
                        distributeCfg.getPort());
                CommonUtil.getInstance().startThread(socketServer);
            }
        }

        return false;
    }

    private DistributeCmd onProcessMessage(String message) {
        return onProcessMsg(message);
    }

    private DistributeCmd onProcessCommand(String command, String cmdParam,
        String expectValue) {
        return onProcessCmd(command, cmdParam, expectValue);
    }

    private DistributeCmd onNotify(String notify) {
        return new DistributeCmd(DistributeDataType.DATA_NOTIFY,
            DistributeConsts.STOP_CMD);
    }

    private DistributeCmd processDistributeCmd(DistributeCmd distributeCmd) {
        if (distributeCmd == null) {
            return new DistributeCmd(DistributeDataType.DATA_MSG,
                DistributeConsts.EMPTY_STR);
        } else {
            String cmdType = distributeCmd.getCmdType().toString();

            if (DistributeDataType.DATA_CMD.toString().equals(cmdType)) {
                return onProcessCommand(distributeCmd.getCommand(),
                    distributeCmd.getParam(), distributeCmd.getExpectValue());
            } else if (DistributeDataType.DATA_MSG.toString().equals(cmdType)) {
                return onProcessMessage(distributeCmd.getCommand());
            } else if (DistributeDataType.DATA_NOTIFY.toString().equals(cmdType)) {
                return onNotify(distributeCmd.getCommand());
            } else {
                CommonUtil.getInstance().info(LOG_AGENT_NAME, "unknown command.");

                return new DistributeCmd(DistributeDataType.DATA_MSG,
                    DistributeConsts.EMPTY_STR);
            }
        }
    }

    /**
     * agent socket
     */
    class SocketServer implements Runnable {
        private String ipAddress = DistributeConsts.EMPTY_STR;
        private int port = 0;
        private ServerSocket serverSocket = null;
        private Socket agentSocket = null;
        private PrintWriter printWriter = null;
        private BufferedReader bufferedReader = null;
        private boolean isStart = false;
        private volatile boolean isTimeout = false;
        private volatile long startTime = 0L;
        private Timer checkTimeout = null;
        private CheckTimeout checkTimeoutTask = null;

        /**
         * agent socket
         *
         * @param ipAddress ip
         * @param port port
         */
        SocketServer(String ipAddress, int port) {
            this.ipAddress = ipAddress;
            this.port = port;
        }

        @Override
        public void run() {
            setUpDistribute();
            initSocketAgent();

            Object receiveObj = null;
            String info = "";

            while (isStart) {
                if (isTimeout || (printWriter == null)) {
                    isStart = false;

                    break;
                }

                try {
                    info = bufferedReader.readLine();

                    if (info == null) {
                        break;
                    }

                    startTime = System.currentTimeMillis();
                    receiveObj = CommonUtil.getInstance().deserializeToObj(info);

                    if (receiveObj == null) {
                        continue;
                    }

                    if (receiveMessage(receiveObj)) {
                        continue;
                    } else {
                        break;
                    }
                } catch (IOException e) {
                    CommonUtil.getInstance()
                        .info(LOG_AGENT_NAME,
                        "bufferedReader readLine failed." + e.getMessage());
                    isStart = false;

                    break;
                }
            }

            receiveObj = null;
            releaseResource();
            tearDownDistribute();
        }

        private boolean receiveMessage(Object receiveObj) {
            DistributeCmd tempCmd = null;

            if (receiveObj instanceof DistributeCmd) {
                tempCmd = processDistributeCmd((DistributeCmd) receiveObj);

                if (tempCmd != null) {
                    return dealReceiveMsg(tempCmd);
                } else {
                    handleNullCmd();
                }
            }

            return true;
        }

        private void handleNullCmd() {
            try {
                printWriter.println(CommonUtil.getInstance()
                    .serializeToStr(new DistributeCmd(
                        DistributeDataType.DATA_NOTIFY,
                        DistributeConsts.EMPTY_STR,
                        DistributeConsts.EMPTY_STR,
                        DistributeConsts.DEFAULT_RESULT)));
                printWriter.flush();
            } catch (IOException e) {
                CommonUtil.getInstance()
                    .info(LOG_AGENT_NAME,
                    "serializeToStr failed." + e.getMessage());
            }
        }

        private boolean dealReceiveMsg(DistributeCmd tempCmd) {
            if (tempCmd.getCmdType() == DistributeDataType.DATA_NOTIFY) {
                if (DistributeConsts.STOP_CMD.equals(tempCmd.getCommand())) {
                    return false;
                }
            } else {
                try {
                    printWriter.println(CommonUtil.getInstance()
                        .serializeToStr(tempCmd));
                    printWriter.flush();
                } catch (IOException e) {
                    CommonUtil.getInstance()
                        .info(LOG_AGENT_NAME,
                        "serializeToStr failed." + e.getMessage());
                }
            }

            return true;
        }

        private void initSocketAgent() {
            try {
                checkTimeout = new Timer();
                checkTimeoutTask = new CheckTimeout();
                checkTimeout.schedule(new CheckTimeout(),
                    DistributeConsts.WAIT_TIME, DistributeConsts.WAIT_TIME);
            } catch (Throwable e) {
                CommonUtil.getInstance()
                    .info(LOG_AGENT_NAME,
                    "create check timeout thread failed." + e.getMessage());
            }

            startTime = System.currentTimeMillis();

            try {
                serverSocket = new ServerSocket();
                serverSocket.bind(new InetSocketAddress(this.ipAddress,
                    this.port));
                serverSocket.setSoTimeout(DistributeConsts.TIME_OUT);
            } catch (IOException e) {
                CommonUtil.getInstance()
                    .info(LOG_AGENT_NAME,
                    "Get ServerSocket failed." + e.getMessage());
                isStart = false;

                return;
            }

            try {
                agentSocket = serverSocket.accept();
                agentSocket.setSoTimeout(DistributeConsts.TIME_OUT);
                printWriter = new PrintWriter(new OutputStreamWriter(
                    agentSocket.getOutputStream()));

                InputStreamReader inputStreamReader = new InputStreamReader(agentSocket.getInputStream());
                bufferedReader = new BufferedReader(inputStreamReader);
            } catch (IOException e) {
                CommonUtil.getInstance()
                    .info(LOG_AGENT_NAME,
                    "Get output or input stream failed." + e.getMessage());
                isStart = false;

                return;
            }

            isStart = true;
        }

        private void releaseResource() {
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    CommonUtil.getInstance()
                        .info(LOG_AGENT_NAME,
                        "close objInputStream failed." + e.getMessage());
                }
            }

            if (printWriter != null) {
                printWriter.close();
            }

            if (checkTimeoutTask != null) {
                try {
                    agentSocket.close();
                } catch (IOException e) {
                    CommonUtil.getInstance()
                        .info(LOG_AGENT_NAME,
                        "close agentSocket failed." + e.getMessage());
                }
            }

            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    CommonUtil.getInstance()
                        .info(LOG_AGENT_NAME,
                        "close serverSocket failed." + e.getMessage());
                }
            }

            if (checkTimeout != null) {
                checkTimeout.cancel();
            }

            if (checkTimeoutTask != null) {
                checkTimeoutTask.cancel();
            }
        }

        /**
         * check timeout
         */
        class CheckTimeout extends TimerTask {
            @Override
            public void run() {
                if ((System.currentTimeMillis() - startTime) > DistributeConsts.TIME_OUT) {
                    isTimeout = true;
                    releaseResource();
                }
            }
        }
    }
}
