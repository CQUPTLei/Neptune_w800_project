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

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

/**
 * distribute test major
 */
public abstract class DistributeMajor implements ISocketObservable {
    /**
     * use map structure to determine device
     */
    private Map<Integer, ISocketObserver> clientMap = new HashMap<Integer, ISocketObserver>();

    private DistributeCfg distributeCfg = null;

    private int coutClient = 0;

    /**
     * default construct
     */
    public DistributeMajor() {
    }

    /**
     * send message from major device to agent device
     *
     * @param devNo device num
     * @param message message to send agent device
     * @param expectValue expect return value
     * @return 0-success,-1-failed
     */
    public int sendMessage(DeviceEnum devNo, String message, String expectValue) {
        if (clientMap != null) {
            ISocketObserver curClient = clientMap.get(devNo.getValue());
            if (curClient instanceof ClientObserver) {
                return ((ClientObserver) curClient).sendMessage(message, expectValue);
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    /**
     * send message from major device to agent device, contains callback
     *
     * @param devNo device num
     * @param message message to send agent device
     * @param expectValue expect return value
     * @param callback callback to handle the return value
     * @return 0-success,-1-failed
     */
    public int sendMessage(DeviceEnum devNo, String message, String expectValue, ICallBack callback) {
        if (clientMap != null) {
            ISocketObserver curClient = clientMap.get(devNo.getValue());
            if (curClient instanceof ClientObserver) {
                return ((ClientObserver) curClient).sendMessage(message, expectValue, callback);
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    /**
     * send command from major device to agent device
     *
     * @param devNo device num
     * @param command command to send agent device
     * @param cmdParam command param
     * @param expectValue expect return value
     * @return 0-success,-1-failed
     */
    public int runCommandOnAgent(DeviceEnum devNo, String command, String cmdParam, String expectValue) {
        if (clientMap != null) {
            ISocketObserver curClient = clientMap.get(devNo.getValue());
            if (curClient instanceof ClientObserver) {
                return ((ClientObserver) curClient).runCommandOnAgent(command, cmdParam, expectValue);
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    /**
     * send command from major device to agent device, contains callback
     *
     * @param devNo device num
     * @param command command to send agent device
     * @param cmdParam command param
     * @param expectValue expect return value
     * @param callback callback to handle the return value
     * @return 0-success,-1-failed
     */
    public int runCommandOnAgent(DeviceEnum devNo, String command, String cmdParam, String expectValue,
        ICallBack callback) {
        if (clientMap != null) {
            ISocketObserver curClient = clientMap.get(devNo.getValue());
            if (curClient instanceof ClientObserver) {
                return ((ClientObserver) curClient).runCommandOnAgent(command, cmdParam, expectValue, callback);
            }
        }
        return DistributeConsts.FAILED_CODE;
    }

    @Override
    public void addObserverSocket(ISocketObserver clientSocket) {
        if (clientSocket instanceof ClientObserver) {
            clientMap.put(DeviceEnum.DEVICE_FIRST.getValue() + coutClient, clientSocket);
        }
        coutClient++;
    }

    @Override
    public void deleteObserverSocket(ISocketObserver clientSocket) {
        clientSocket.updateSocket(DistributeConsts.STOP_CMD);
        if (clientMap != null) {
            clientMap.remove(clientSocket);
        }
        coutClient--;
    }

    @Override
    public void notifyObservers(String message) {
        if (clientMap != null) {
            Collection<ISocketObserver> allSockets = clientMap.values();
            for (ISocketObserver clientSocket : allSockets) {
                clientSocket.updateSocket(message);
            }
        }
    }

    @Override
    public boolean initEnvironment() {
        distributeCfg = new DistributeCfg();
        distributeCfg.readDistributeCfg("major.desc");
        if (distributeCfg != null) {
            for (String ipAddress : distributeCfg.getIpList()) {
                ClientObserver clientObserver = new ClientObserver(ipAddress, distributeCfg.getPort());
                addObserverSocket(clientObserver);
            }
            notifyObservers(DistributeConsts.START_CMD);
            return true;
        }
        return false;
    }

    @Override
    public boolean releaseEnvironment() {
        notifyObservers(DistributeConsts.STOP_CMD);
        if (clientMap != null) {
            Collection<ISocketObserver> allSockets = clientMap.values();
            for (ISocketObserver clientSocket : allSockets) {
                clientMap.remove(clientSocket);
            }
            clientMap.clear();
            return true;
        }
        return false;
    }

    public Map<Integer, ISocketObserver> getClientMap() {
        return clientMap;
    }

    public void setClientMap(Map<Integer, ISocketObserver> clientMap) {
        this.clientMap = clientMap;
    }
}