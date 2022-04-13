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

package ohos.unittest.perf;

/**
 * 〈performance verify〉
 */
public class PerfVerify {
    private static final int DATA_3 = 3;

    /**
     * baseLine
     */
    private BaseLine perfBaseLine = null;

    /**
     * version
     */
    private long version = 1;

    /**
     * constructor
     *
     * @param baseLine baseLine
     */
    public PerfVerify(BaseLine baseLine) {
        this.perfBaseLine = baseLine;
        this.version = 1;
    }

    /**
     * constructor
     *
     * @param baseLine baseLine
     * @param version baseline version
     */
    public PerfVerify(BaseLine baseLine, long version) {
        this.perfBaseLine = baseLine;
        this.version = version;
    }

    /**
     * expectLarger
     *
     * @apiNote expectLarger
     * @param spentTime spentTime
     * @param version version
     * @return boolean
     */
    public boolean expectLarger(double spentTime, long version) {
        this.version = version;
        String caseName = Thread.currentThread().getStackTrace()[DATA_3].getMethodName();
        String className = Thread.currentThread().getStackTrace()[DATA_3].getClassName();
        return VerifySingleton.getInstance().expectLarger(className, caseName, spentTime,
        this.perfBaseLine, this.version);
    }

    /**
     * expectLarger
     *
     * @apiNote expectLarger
     * @param spentTime spentTime
     * @return boolean
     */
    public boolean expectLarger(double spentTime) {
        String caseName = Thread.currentThread().getStackTrace()[DATA_3].getMethodName();
        String className = Thread.currentThread().getStackTrace()[DATA_3].getClassName();
        return VerifySingleton.getInstance().expectLarger(className, caseName, spentTime,
        this.perfBaseLine, this.version);
    }

    /**
     * expectSmaller
     *
     * @apiNote expectSmaller
     * @param spentTime spentTime
     * @param version version
     * @return boolean
     */
    public boolean expectSmaller(double spentTime, long version) {
        this.version = version;
        String caseName = Thread.currentThread().getStackTrace()[DATA_3].getMethodName();
        String className = Thread.currentThread().getStackTrace()[DATA_3].getClassName();
        return VerifySingleton.getInstance().expectSmaller(className, caseName, spentTime,
        this.perfBaseLine, this.version);
    }

    /**
     * expectSmaller
     *
     * @apiNote expectSmaller
     * @param spentTime spentTime
     * @return boolean
     */
    public boolean expectSmaller(double spentTime) {
        String caseName = Thread.currentThread().getStackTrace()[DATA_3].getMethodName();
        String className = Thread.currentThread().getStackTrace()[DATA_3].getClassName();
        return VerifySingleton.getInstance().expectSmaller(className, caseName, spentTime,
        this.perfBaseLine, this.version);
    }

    @Override
    public String toString() {
        return "Verify [perfBaseLine=" + perfBaseLine + ", version=" + version + "]";
    }
}
