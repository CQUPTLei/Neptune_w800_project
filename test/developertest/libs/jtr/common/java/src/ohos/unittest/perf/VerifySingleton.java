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

import ohos.unittest.log.Logger;

import org.junit.Assert;

import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Map.Entry;

/**
 * 〈performance verify Singleton〉
 */
public class VerifySingleton {
    /**
     * performance test result
     */
    private Map<String /* suit name */, Map<String /* case name */, PerfTest>> suitPerfResult = new HashMap<>();

    /**
     * Verify constructor
     */
    private VerifySingleton() {
    }

    /**
     * Singleton
     */
    private static class Singleton {
        static VerifySingleton instance = new VerifySingleton();
    }

    /**
     * getInstance
     *
     * @apiNote getInstance
     * @return VerifyMgr Singleton
     */
    public static VerifySingleton getInstance() {
        return Singleton.instance;
    }

    public Map<String, Map<String, PerfTest>> getSuitPerfResult() {
        return suitPerfResult;
    }

    /**
     * isPerfTestcase
     *
     * @apiNote isPerfTestcase
     * @param className test class Name
     * @param caseName test case Name
     * @return boolean
     */
    public boolean isPerfTestcase(String className, String caseName) {
        PerfTest perfTest = getTestCasePerfResult(className, caseName);
        if (perfTest == null) {
            return false;
        }
        return true;
    }

    private PerfTest getTestCasePerfResult(String className, String caseName) {
        PerfTest perfTest = null;
        for (Entry<String, Map<String, PerfTest>> entry : suitPerfResult.entrySet()) {
            if (!className.equals(entry.getKey())) {
                continue;
            }

            for (Entry<String, PerfTest> entry1 : entry.getValue().entrySet()) {
                if (!caseName.equals(entry1.getKey())) {
                    continue;
                }
                perfTest = entry1.getValue();
            }

            if (perfTest != null) {
                break;
            }
        }
        return perfTest;
    }

    /**
     * getPerfTestResult
     *
     * @apiNote getPerfTestResult
     * @param className test class Name
     * @param caseName test case Name
     * @return Map
     */
    public Map<String, String> getPerfTestResult(String className, String caseName) {
        Map<String, String> perfResult = new HashMap<>();
        PerfTest perfTest = getTestCasePerfResult(className, caseName);
        if (perfTest == null) {
            return perfResult;
        }

        perfResult.put("value", String.valueOf(perfTest.getTestValue()));
        perfResult.put("category", String.valueOf(perfTest.getCategory()));
        perfResult.put("tc_version", String.valueOf(perfTest.getCaseVersion()));
        perfResult.put("lastvalue", String.valueOf(perfTest.getDbLastValue()));
        perfResult.put("baseline", String.valueOf(perfTest.getDbBaseLineValue()));
        return perfResult;
    }

    /**
     * expectLarger
     *
     * @apiNote expectLarger
     * @param className test class Name
     * @param caseName test case Name
     * @param spentTime spentTime
     * @param baseLine performance baseLine
     * @param version version
     * @return boolean
     */
    public boolean expectLarger(String className, String caseName, double spentTime, BaseLine baseLine, long version) {
        PerfTest perfTest = new PerfTest(className, caseName, baseLine.getBaseLineParser(), version);
        Map<String, PerfTest> defaultValue = suitPerfResult.get(className);
        if (defaultValue == null) {
            defaultValue = new HashMap<>();
        }
        defaultValue.put(caseName, perfTest);
        suitPerfResult.put(className, defaultValue);
        return perfTest.expectLarger(spentTime);
    }

    /**
     * expectSmaller
     *
     * @apiNote expectSmaller
     * @param className test class Name
     * @param caseName test case Name
     * @param spentTime spentTime
     * @param baseLine performance baseLine
     * @param version version
     * @return boolean
     */
    public boolean expectSmaller(String className, String caseName, double spentTime, BaseLine baseLine, long version) {
        PerfTest perfTest = new PerfTest(className, caseName, baseLine.getBaseLineParser(), version);
        Map<String, PerfTest> defaultValue = suitPerfResult.get(className);
        if (defaultValue == null) {
            defaultValue = new HashMap<>();
        }
        defaultValue.put(caseName, perfTest);
        suitPerfResult.put(className, defaultValue);
        return perfTest.expectSmaller(spentTime);
    }

    @Override
    public String toString() {
        return "VerifyMgr [suitPerfResult=" + suitPerfResult + "]";
    }
}

/**
 * PerfTest
 */
class PerfTest {
    private String caseName = "";

    private String className = "";

    private String category = "performance";

    private boolean isHasBaseLine = false;

    private boolean isHasLastValue = false;

    private boolean isHasFloatRange = false;

    private double testValue = 0.0;

    private double dbLastValue = 0.0;

    private double dbBaseLineValue = 0.0;

    private double dbFloatRange = 0.0;

    private long caseVersion = 0L;

    private BaseLineParser perfBaseLine = null;

    /**
     * PerfTest constructor
     *
     * @param baseLine Performance baseLine object
     * @param version Performance baseLine version
     */
    PerfTest(String className, String caseName, BaseLineParser baseLine, long version) {
        this.className = className;
        this.caseName = caseName;
        this.perfBaseLine = baseLine;
        this.caseVersion = version;
        init();
    }

    public long getCaseVersion() {
        return caseVersion;
    }

    public void setCaseVersion(long caseVersion) {
        this.caseVersion = caseVersion;
    }

    public BaseLineParser getPerfBaseLine() {
        return perfBaseLine;
    }

    public void setPerfBaseLine(BaseLineParser perfBaseLine) {
        this.perfBaseLine = perfBaseLine;
    }

    public String getCategory() {
        return category;
    }

    public void setCategory(String category) {
        this.category = category;
    }

    public double getDbLastValue() {
        return dbLastValue;
    }

    public void setDbLastValue(double dbLastValue) {
        this.dbLastValue = dbLastValue;
    }

    public double getDbBaseLineValue() {
        return dbBaseLineValue;
    }

    public void setDbBaseLineValue(double dbBaseLineValue) {
        this.dbBaseLineValue = dbBaseLineValue;
    }

    public double getDbFloatRange() {
        return dbFloatRange;
    }

    public void setDbFloatRange(double dbFloatRange) {
        this.dbFloatRange = dbFloatRange;
    }

    public String getCaseName() {
        return caseName;
    }

    public void setCaseName(String caseName) {
        this.caseName = caseName;
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

    public boolean isHasBaseLine() {
        return isHasBaseLine;
    }

    public void setHasBaseLine(boolean isBaseLine) {
        this.isHasBaseLine = isBaseLine;
    }

    public boolean isHasLastValue() {
        return isHasLastValue;
    }

    public void setHasLastValue(boolean isLastValue) {
        this.isHasLastValue = isLastValue;
    }

    public boolean isHasFloatRange() {
        return isHasFloatRange;
    }

    public void setHasFloatRange(boolean isFloatRange) {
        this.isHasFloatRange = isFloatRange;
    }

    public double getTestValue() {
        return testValue;
    }

    public void setTestValue(double testValue) {
        this.testValue = testValue;
    }

    /**
     * expectLarger
     *
     * @apiNote expectLarger
     * @param spentTime spentTime
     * @return boolean
     */
    public boolean expectLarger(double spentTime) {
        return expectValue(spentTime, true);
    }

    /**
     * expectSmaller
     *
     * @apiNote expectSmaller
     * @param spentTime spentTime
     * @return boolean
     */
    public boolean expectSmaller(double spentTime) {
        return expectValue(spentTime, false);
    }

    /**
     * saveResult
     *
     * @apiNote saveResult
     * @param spentTime spentTime
     */
    public void saveResult(double spentTime) {
    }

    /**
     * initialize
     *
     * @apiNote init
     * @return boolean
     */
    private boolean init() {
        if (caseName == null || perfBaseLine == null || caseName.isEmpty()) {
            return false;
        }

        isHasBaseLine = perfBaseLine.isHasBaseLineValue(caseName);
        if (!isHasBaseLine) {
            return false;
        } else {
            dbBaseLineValue = perfBaseLine.getBaseLineValue(caseName);
        }

        dbLastValue = perfBaseLine.getLastValue(caseName);

        isHasLastValue = perfBaseLine.isHasLastValue(caseName);

        isHasFloatRange = perfBaseLine.isHasFloatRange(caseName);
        if (isHasFloatRange) {
            dbFloatRange = perfBaseLine.getFloatRange(caseName);
        }

        if (isHasFloatRange && (dbFloatRange < 0 || dbFloatRange >= 1)) {
            Logger.info(String.format(Locale.ROOT, "[ ERROR    ] %s has invalid float range: %s", caseName,
                String.valueOf(dbFloatRange)));
            isHasFloatRange = false;
        }

        if (!isHasFloatRange) {
            dbFloatRange = 0.0;
            Logger.info(String.format(Locale.ROOT, "[ WARING   ] %s has NO FloatRange, default is %s", caseName,
                String.valueOf(dbFloatRange)));
        }

        if (!isHasLastValue) {
            dbLastValue = dbBaseLineValue;
            Logger.info(String.format(Locale.ROOT, "[ WARING   ] %s has NO LastValue, default is %s", caseName,
                String.valueOf(dbLastValue)));
        }
        return true;
    }

    /**
     * showBaselineInfo
     *
     * @apiNote showBaselineInfo
     */
    public void showBaselineInfo() {
        Logger.info(String.format(Locale.ROOT, "[ INFO   ] %s default dbLastValue is %s", caseName,
            String.valueOf(dbLastValue)));

        Logger.info(String.format(Locale.ROOT, "[ INFO   ] %s default dbFloatRange is %s", caseName,
            String.valueOf(dbFloatRange)));

        Logger.info(String.format(Locale.ROOT, "[ INFO   ] %s default caseVersion is %s", caseName,
            String.valueOf(caseVersion)));

        Logger.info(String.format(Locale.ROOT, "[ INFO   ] %s default dbBaseLineValue is %s", caseName,
            String.valueOf(dbBaseLineValue)));
    }

    private boolean expectValue(double value, boolean isLargerBetter) {
        Assert.assertTrue("expect true, but is false.", isTestCase());
        boolean isTestSuccess = false;
        testValue = value;

        if (perfBaseLine != null && !perfBaseLine.isHasBaselineConfig()) {
            isTestSuccess = true;
        } else if (!isHasBaseLine) {
            isTestSuccess = false;
        } else if (isLargerBetter) {
            double baseValue = Math.max(dbLastValue, dbBaseLineValue);
            isTestSuccess = testValue >= baseValue * (1.0 - dbFloatRange) ? true : false;
        } else {
            double baseValue = Math.min(dbLastValue, dbBaseLineValue);
            isTestSuccess = testValue <= baseValue * (1.0 + dbFloatRange) ? true : false;
        }
        Logger.info(String.format(Locale.ROOT, "[ PERF     ] %s.%s: baseline:%f, test_result: %f", className, caseName,
            dbBaseLineValue, value));
        Assert.assertTrue("expect true, but is false.", isTestSuccess);
        return isTestSuccess;
    }

    private boolean isTestCase() {
        if (className == null || className.isEmpty()) {
            Logger.info(String.format(Locale.ROOT,
                "[ ERROR    ] Failed to get test class name, class name can't be empty."));
            return false;
        }

        if (caseName == null || caseName.isEmpty()) {
            Logger.info(String.format(Locale.ROOT,
                "[ ERROR    ] Failed to get test case name, test case name can't be empty."));
            return false;
        }

        Class<?> testClass;
        try {
            testClass = Class.forName(className);
            if (testClass != null) {
                List<String> listTestCase = getAllTestCase(testClass);
                if (listTestCase.contains(caseName)) {
                    return true;
                } else {
                    Logger.info(String.format(Locale.ROOT,
                        "[ ERROR    ] %s Performance verification can only be called in test case functions",
                        className));
                }
            }
        } catch (ClassNotFoundException e) {
            Logger.info("[ ERROR    ] Fail to get class by name: " + e.getMessage());
        }

        return false;
    }

    @SuppressWarnings("unchecked")
    private List<String> getAllTestCase(Class<?> clazz) {
        List<String> listTestCase = new ArrayList<>();
        Class<? extends Annotation> junitIgnore;
        Class<? extends Annotation> junitTest;
        try {
            junitIgnore = (Class<? extends Annotation>) Class.forName("org.junit.Ignore");
            junitTest = (Class<? extends Annotation>) Class.forName("org.junit.Test");
        } catch (ClassNotFoundException e) {
            Logger.info("[ ERROR    ] Fail to get Class by name: " + e.getMessage());
            return listTestCase;
        }

        Method[] methods = clazz.getDeclaredMethods();
        for (Method method : methods) {
            boolean isTestAnno = method.isAnnotationPresent(junitTest);
            boolean isIgnoreAnno = method.isAnnotationPresent(junitIgnore);
            if (isTestAnno && !isIgnoreAnno) {
                listTestCase.add(method.getName());
            }
        }
        return listTestCase;
    }

    @Override
    public String toString() {
        return "PerfTest [caseName=" + caseName + ", className=" + className + ", category=" + category
            + ", isHasBaseLine=" + isHasBaseLine + ", isHasLastValue=" + isHasLastValue + ", isHasFloatRange="
            + isHasFloatRange + ", testValue=" + testValue + ", dbLastValue=" + dbLastValue + ", dbBaseLineValue="
            + dbBaseLineValue + ", dbFloatRange=" + dbFloatRange + ", caseVersion=" + caseVersion + ", perfBaseLine="
            + perfBaseLine + "]";
    }
}
