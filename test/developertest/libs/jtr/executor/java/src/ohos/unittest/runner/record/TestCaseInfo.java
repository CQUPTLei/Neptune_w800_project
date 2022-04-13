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

package ohos.unittest.runner.record;

/**
 * Test Case Information
 */
public class TestCaseInfo {
    /**
     * Test method execution begin time, in milliseconds
     */
    private long startTime = 0L;

    /**
     * Test method execution end time, in milliseconds
     */
    private long endTime = 0L;

    /**
     * Test method execution total time, in milliseconds
     */
    private long totalTime = 0L;

    /**
     * Test case level
     */
    private String level = "";

    /**
     * Test class name
     */
    private String className = "";

    /**
     * Test method(case) name
     */
    private String name = "";

    /**
     * Test method execution result(true/false)
     */
    private boolean isSuccess = true;

    /**
     * Test method ignored flag(true/false)
     */
    private boolean isIgnored = false;

    /**
     * Test method execution errorMsg
     */
    private String errorMsg = "";

    /**
     * Test method execution traceInfo
     */
    private String traceInfo = "";

    /**
     * test Target Class name
     */
    private String testTargetClass = "";

    /**
     * test Target Method name
     */
    private String testTargetMethod = "";

    /**
     * CaseInfo Constructor
     */
    public TestCaseInfo() {
        super();
        this.name = "";
        this.level = "";
        this.className = "";
        this.startTime = 0;
        this.endTime = 0;
        this.totalTime = 0;
        this.errorMsg = "";
        this.testTargetClass = "";
        this.testTargetMethod = "";
        this.isIgnored = false;
        this.isSuccess = true;
    }

    public long getStartTime() {
        return startTime;
    }

    public void setStartTime(long startTime) {
        this.startTime = startTime;
    }

    public long getEndTime() {
        return endTime;
    }

    public void setEndTime(long endTime) {
        this.endTime = endTime;
    }

    /**
     * Get case total execution time
     *
     * @return total execution time
     */
    public long getTotalTime() {
        totalTime = endTime - startTime;
        return totalTime;
    }

    public void setTotalTime(long totalTime) {
        this.totalTime = totalTime;
    }

    public String getLevel() {
        return level;
    }

    public void setLevel(String level) {
        this.level = level;
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean isSuccess() {
        return isSuccess;
    }

    public void setSuccess(boolean isSuccessed) {
        this.isSuccess = isSuccessed;
    }

    public boolean isIgnored() {
        return isIgnored;
    }

    public void setIgnored(boolean isIgnoredFlag) {
        this.isIgnored = isIgnoredFlag;
    }

    public String getErrorMsg() {
        return errorMsg;
    }

    public void setErrorMsg(String errorMsg) {
        this.errorMsg = errorMsg;
    }

    public String getTraceInfo() {
        return traceInfo;
    }

    public void setTraceInfo(String traceInfo) {
        this.traceInfo = traceInfo;
    }

    public String getTestTargetMethod() {
        return testTargetMethod;
    }

    public void setTestTargetMethod(String testTargetMethod) {
        this.testTargetMethod = testTargetMethod;
    }

    public String getTestTargetClass() {
        return testTargetClass;
    }

    public void setTestTargetClass(String testTargetClass) {
        this.testTargetClass = testTargetClass;
    }

    @Override
    public String toString() {
        return "TestCaseInfo [startTime=" + startTime + ", endTime=" + endTime + ", totalTime=" + totalTime + ", level="
            + level + ", className=" + className + ", name=" + name + ", isSuccess=" + isSuccess + ", isIgnored="
            + isIgnored +  ", testTargetClass=" + testTargetClass + ", testTargetMethod=" + testTargetMethod
            + ", errorMsg=" + errorMsg + ", traceInfo=" + traceInfo + "]";
    }
}