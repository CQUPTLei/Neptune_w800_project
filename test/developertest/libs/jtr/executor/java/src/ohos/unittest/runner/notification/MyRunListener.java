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

package ohos.unittest.runner.notification;

import ohos.unittest.CaseLevel;
import ohos.unittest.Level;
import ohos.unittest.TestTarget;
import ohos.unittest.log.Logger;
import ohos.unittest.runner.record.TestCaseInfo;
import ohos.unittest.runner.record.TestClassInfo;

import org.junit.Ignore;
import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

/**
 * User defined Junit RunListener
 */
public class MyRunListener extends RunListener {
    /**
     * Test class execution result record
     */
    private TestClassInfo testClassInfo = null;

    /**
     * Test class test case execution temporary result record
     */
    private TestCaseInfo testCaseInfo = null;

    /**
     * Test class execution result record
     */
    private List<TestCaseInfo> allRanTestCaseInfo = null;

    /**
     * Case level map
     */
    private Map<Level, String> mapLevel = null;

    /**
     * MyRunListener Constructor
     */
    public MyRunListener() {
        this.allRanTestCaseInfo = new ArrayList<>();
        this.mapLevel = new HashMap<>();
        this.mapLevel.put(Level.DEFAULT, "");
        this.mapLevel.put(Level.LEVEL_0, "0");
        this.mapLevel.put(Level.LEVEL_1, "1");
        this.mapLevel.put(Level.LEVEL_2, "2");
        this.mapLevel.put(Level.LEVEL_3, "3");
        this.mapLevel.put(Level.LEVEL_4, "4");
    }

    @Override
    public void testRunStarted(Description description) throws Exception {
        testClassInfo = new TestClassInfo();
    }

    @Override
    public void testRunFinished(Result result) throws Exception {
        testClassInfo.setSuccess(result.wasSuccessful());
        testClassInfo.setRunTime(result.getRunTime());

        int runCount = result.getRunCount();
        testClassInfo.setRunCount(runCount);

        int ignoreCount = result.getIgnoreCount();
        testClassInfo.setIgnoreCount(ignoreCount);

        testClassInfo.setAllRanTestCaseInfo(allRanTestCaseInfo);

        String className = testClassInfo.getClassName();
        long runTime = testClassInfo.getRunTime();

        boolean isRunException = false;
        if (runCount <= 0) {
            isRunException = true;
        }

        if (isRunException) {
            Logger.info(
                String.format(Locale.ROOT, "[==========] %s", "Testsuit running exception, please check your code!"));
        }

        Logger.info(String.format(Locale.ROOT, "[==========] %s tests from %s (%s ms total)", String.valueOf(runCount),
            className, String.valueOf(runTime)));

        int passedCount = 0;
        int failureCount = testClassInfo.getFailureCount();
        if (runCount >= failureCount) {
            passedCount = runCount - failureCount;
        }
        Logger.info(String.format(Locale.ROOT, "[  PASSED  ] %s tests.", String.valueOf(passedCount)));

        if (runCount >= failureCount && failureCount > 0) {
            Logger.info(String.format(Locale.ROOT, "[  FAILED  ] %s tests.", String.valueOf(failureCount)));
        }
        if (ignoreCount > 0) {
            Logger.info(String.format(Locale.ROOT, "[  IGNORED ] %s tests.", String.valueOf(ignoreCount)));
        }
    }

    public TestClassInfo getTestClassInfo() {
        return testClassInfo;
    }

    public void setTestClassInfo(TestClassInfo testClassInfo) {
        this.testClassInfo = testClassInfo;
    }

    public TestCaseInfo getTestCaseInfo() {
        return testCaseInfo;
    }

    public void setTestCaseInfo(TestCaseInfo testCaseInfo) {
        this.testCaseInfo = testCaseInfo;
    }

    public List<TestCaseInfo> getAllRanTestCaseInfo() {
        return allRanTestCaseInfo;
    }

    public void setAllRanTestCaseInfo(List<TestCaseInfo> allRanTestCaseInfo) {
        this.allRanTestCaseInfo = allRanTestCaseInfo;
    }

    public Map<Level, String> getMapLevel() {
        return mapLevel;
    }

    public void setMapLevel(Map<Level, String> mapLevel) {
        this.mapLevel = mapLevel;
    }

    @Override
    public void testStarted(Description description) throws Exception {
        iniTestCaseInfo(description);

        Logger.info(
            String.format(Locale.ROOT, "[ RUN      ] %s.%s", description.getClassName(), description.getMethodName()));
    }

    private void iniTestCaseInfo(Description description) {
        Ignore ignoreCase = description.getAnnotation(Ignore.class);
        if (ignoreCase != null) {
            return;
        }

        testCaseInfo = new TestCaseInfo();
        testCaseInfo.setStartTime(System.currentTimeMillis());
        CaseLevel caseLevel = description.getAnnotation(CaseLevel.class);
        if (caseLevel != null && mapLevel.containsKey(caseLevel.level())) {
            testCaseInfo.setLevel(mapLevel.get(caseLevel.level()));
        } else {
            testCaseInfo.setLevel("undefined");
        }

        TestTarget testTarget = description.getAnnotation(TestTarget.class);
        if (testTarget != null) {
            testCaseInfo.setTestTargetClass(testTarget.className());
            testCaseInfo.setTestTargetMethod(testTarget.methodName());
        }

        testCaseInfo.setName(description.getMethodName());
        testCaseInfo.setClassName(description.getClassName());
        if (testClassInfo != null) {
            String strClassName = description.getClassName();
            testClassInfo.setClassName(strClassName);
        }
        allRanTestCaseInfo.add(testCaseInfo);
    }

    @Override
    public void testFinished(Description description) throws Exception {
        if (testCaseInfo != null) {
            testCaseInfo.setEndTime(System.currentTimeMillis());
            if (testCaseInfo.isSuccess()) {
                Logger.info(String.format(Locale.ROOT, "[       OK ] %s.%s (%s ms)", description.getClassName(),
                    description.getMethodName(), String.valueOf(testCaseInfo.getTotalTime())));
            } else {
                if (testClassInfo != null) {
                    testClassInfo.setFailureCount(testClassInfo.getFailureCount() + 1);
                }
                Logger.info(String.format(Locale.ROOT, "[   FAILED ] %s.%s (%s ms)", description.getClassName(),
                    description.getMethodName(), String.valueOf(testCaseInfo.getTotalTime())));
                Logger.info(testCaseInfo.getTraceInfo());
            }
        }
    }

    @Override
    public void testFailure(Failure failure) throws Exception {
        if (testCaseInfo != null) {
            testCaseInfo.setSuccess(false);
            if (failure != null && failure.getMessage() != null) {
                testCaseInfo.setErrorMsg(failure.getMessage());
            } else {
                testCaseInfo.setErrorMsg("Message is empty");
            }
            if (failure != null && failure.getTrace() != null) {
                testCaseInfo.setTraceInfo(failure.getTrace());
            }
            testCaseInfo.setEndTime(System.currentTimeMillis());
        } else {
            if (failure != null && failure.getMessage() != null) {
                Logger.info(String.format(Locale.ROOT, "[   ERROR  ] %s", failure.getMessage()));
                if (testClassInfo != null) {
                    testClassInfo.setSuccess(false);
                    testClassInfo.setErrorMessage(failure.getMessage());
                }
            }
        }
    }

    @Override
    public void testIgnored(Description description) throws Exception {
        iniTestCaseInfo(description);
        if (testCaseInfo != null) {
            testCaseInfo.setEndTime(System.currentTimeMillis());
        }
        Logger.info(String.format(Locale.ROOT, "[----------] %s.%s Ignored", description.getClassName(),
            description.getMethodName()));
    }

    @Override
    public String toString() {
        return "ZRunListener [testClassInfo=" + testClassInfo + ", testCaseInfo=" + testCaseInfo
            + ", allRanTestCaseInfo=" + allRanTestCaseInfo + ", mapLevel=" + mapLevel + "]";
    }
}
