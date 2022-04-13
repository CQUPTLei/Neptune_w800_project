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
 * Client for distribute socket
 */
public interface ISocketObserver {
    /**
     * init test resources
     *
     * @apiNote init test resources
     *
     * @return whether init is successful
     */
    boolean initEnvironment();

    /**
     * update all socket messages
     *
     * @apiNote update all socket messages
     *
     * @param command processed command
     */
    void updateSocket(String command);

    /**
     * release test resources
     *
     * @apiNote release test resources
     *
     */
    void releaseEnvironment();

    /**
     * get ipAddress
     *
     * @apiNote get ipAddress
     *
     * @return ipAddress
     */
    String getIpAddress();

    /**
     * get port
     *
     * @apiNote get port
     *
     * @return port
     */
    int getPort();
}
