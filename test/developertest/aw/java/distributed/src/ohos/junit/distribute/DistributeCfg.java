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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

/**
 * distribute configuration
 */
public class DistributeCfg {
    private static final String LOG_CFG_NAME = "DistributeCfg";

    /**
     * agent port
     */
    private int port = 0;

    /**
     * agent ipAddress
     */
    private List<String> ipList = new ArrayList<String>();

    /**
     * create configuration
     *
     */
    public DistributeCfg() {
    }

    public List<String> getIpList() {
        return this.ipList;
    }

    /**
     * add ipAddress
     *
     * @apiNote add ipAddress
     *
     * @param ipAddress ipAddress
     */
    public void addIpAddress(String ipAddress) {
        if (ipAddress == null || ipAddress.isEmpty()) {
            CommonUtil.getInstance().info(LOG_CFG_NAME, "ipAddress is null.");
        } else {
            this.ipList.add(ipAddress);
        }
    }

    public int getPort() {
        return this.port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    /**
     * read distribute configuration info
     *
     * @param fileName configuration
     */
    public void readDistributeCfg(String fileName) {
        if (fileName == null || fileName.isEmpty()) {
            CommonUtil.getInstance().info(LOG_CFG_NAME, "configuration file not exists.");
        } else {
            File cfgFile = new File(CommonUtil.getInstance().getCurrentPath() + File.separator + fileName);
            CommonUtil.getInstance().info(LOG_CFG_NAME, "cfg path=" + cfgFile.getAbsolutePath());
            if (CommonUtil.getInstance().isFileAndExist(cfgFile)) {
                try (InputStreamReader reader =
                    new InputStreamReader(new FileInputStream(cfgFile), DistributeConsts.DEFAULT_ENCODING)) {
                    try (BufferedReader bufferReader = new BufferedReader(reader)) {
                        handleConfigInfo(bufferReader);
                    } catch (IOException e) {
                        CommonUtil.getInstance().info(LOG_CFG_NAME, "read configuration failed." + e.getMessage());
                    }
                } catch (IOException e1) {
                    CommonUtil.getInstance().info(LOG_CFG_NAME, "read configuration failed." + e1.getMessage());
                }
            }
        }
    }

    private void handleConfigInfo(BufferedReader bufferReader) throws IOException {
        String cfgInfo = null;
        while ((cfgInfo = bufferReader.readLine()) != null) {
            if (cfgInfo.contains(DistributeConsts.COLON_SIGLE)) {
                String[] readInfo = cfgInfo.split(DistributeConsts.COLON_SIGLE);
                switch (readInfo[0]) {
                    case DistributeConsts.IP_CFG_KEY:
                        if (readInfo[1].contains(DistributeConsts.DOT_SIGLE)) {
                            String[] ipAddressList = readInfo[1].split(DistributeConsts.DOT_SIGLE);
                            for (String ipAddress : ipAddressList) {
                                this.ipList.add(ipAddress);
                            }
                        } else {
                            this.ipList.add(readInfo[1]);
                        }
                        break;
                    case DistributeConsts.PORT_CFG_KEY:
                        this.port = Integer.valueOf(readInfo[1]);
                        break;
                    default:
                        CommonUtil.getInstance().info(LOG_CFG_NAME, "error configuration.");
                }
            }
        }
    }
}