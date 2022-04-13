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

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

/**
 * Test class test method execution result statistics
 */
public class TestClassInfo {
    /**
     * start time for test class execution
     */
    private String startTime;

    /**
     * Test class
     */
    private String className = "";

    /**
     * errorMessage for test class execution
     */
    private String errorMessage;

    /**
     * Test function execution result list
     */
    private List<TestCaseInfo> allRanTestCaseInfo = null;

    /**
     * Test class test result
     */
    private boolean isSuccess = false;

    /**
     * Test class all use case execution time total, in milliseconds
     */
    private long runTime = 0L;

    /**
     * Test class test function execution quantity
     */
    private int runCount = 0;

    /**
     * Test class test function execution failure quantity
     */
    private int failureCount = 0;

    /**
     * Test class test function execution ignores quantity
     */
    private int ignoreCount = 0;

    /**
     * TestClassInfo Constructor
     */
    public TestClassInfo() {
        super();
        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

        this.startTime = df.format(new Date());
    }

    public List<TestCaseInfo> getAllRanTestCaseInfo() {
        return allRanTestCaseInfo;
    }

    public void setAllRanTestCaseInfo(List<TestCaseInfo> allRanTestCaseInfo) {
        this.allRanTestCaseInfo = allRanTestCaseInfo;
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

    public boolean isSuccess() {
        return isSuccess;
    }

    public void setSuccess(boolean isSuccessed) {
        this.isSuccess = isSuccessed;
    }

    public long getRunTime() {
        return runTime;
    }

    public void setRunTime(long runTime) {
        this.runTime = runTime;
    }

    public int getRunCount() {
        return runCount;
    }

    public void setRunCount(int runCount) {
        this.runCount = runCount;
    }

    public int getFailureCount() {
        return failureCount;
    }

    public void setFailureCount(int failureCount) {
        this.failureCount = failureCount;
    }

    public int getIgnoreCount() {
        return ignoreCount;
    }

    public void setIgnoreCount(int ignoreCount) {
        this.ignoreCount = ignoreCount;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public String getErrorMessage() {
        return errorMessage;
    }

    public void setErrorMessage(String errorMessage) {
        this.errorMessage = errorMessage;
    }

    @Override
    public String toString() {
        return "TestClassInfo [startTime=" + startTime + ", className=" + className + ", errorMessage=" + errorMessage
            + ", allRanTestCaseInfo=" + allRanTestCaseInfo + ", isSuccess=" + isSuccess + ", runTime=" + runTime
            + ", runCount=" + runCount + ", failureCount=" + failureCount + ", ignoreCount=" + ignoreCount + "]";
    }

}
