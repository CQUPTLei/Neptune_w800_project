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

package ohos.unittest.runner;

import ohos.unittest.Level;
import ohos.unittest.log.Logger;
import ohos.unittest.log.LogFile;
import ohos.unittest.runner.record.TestClassInfo;
import ohos.unittest.runner.notification.MyCaseListener;
import ohos.unittest.runner.notification.MyRunListener;

import org.junit.runner.JUnitCore;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Locale;
import java.util.List;

/**
 * java UT Test Runner
 */
public class UtTestRunner extends TestRunner {
    public UtTestRunner() {
    }

    /**
     * execute single class
     *
     * @param strClassName class name needed to execute
     * @return all TestClassInfo list
     * @throws IOException
     */
    public List<TestClassInfo> runClassByNameImp(String strClassName) throws IOException {
        List<TestClassInfo> listRecorderTmp = new ArrayList<>();
        try {
            Class<?> testClass = Class.forName(strClassName);
            if (testClass != null) {
                Logger.info("");
                Logger.info(String.format(Locale.ROOT, "[==========] Running tests from %s", strClassName));
                listRecorderTmp = runClasses(testClass);
                Logger.info("");
            }
        } catch (ClassNotFoundException e) {
            Logger.info("runClassByName Exception with message:" + e.getMessage());
        }
        return listRecorderTmp;
    }

    /**
     * Test running by JunitCore
     *
     * @param classes list classes
     * @return TestClassInfo list
     */
    private List<TestClassInfo> runClasses(Class<?>... classes) {
        List<TestClassInfo> listTestClassInfo = new ArrayList<>();

        // Execute Junit test class
        for (Class<?> clazz : classes) {
            JUnitCore junitCore = new JUnitCore();

            // Add Listener for JUnitCore to monitor Junit test class Execute results
            MyRunListener listener = new MyRunListener();
            junitCore.addListener(listener);

            // Add ZRunCase for JUnitCore to decide witch test case should be executed
            MyCaseListener caseListener = new MyCaseListener();
            caseListener.setExecClassName(super.getExecClassName());
            caseListener.setExecMethodName(super.getExecMethodName());
            caseListener.setExecCaseLevel(super.getExecCaseLevel());
            junitCore.setCaseListener(caseListener);

            // Launch Junit test class running
            junitCore.run(clazz);
            String className = clazz.getName();

            TestClassInfo testClassInfo = listener.getTestClassInfo();
            testClassInfo.setClassName(className);

            listTestClassInfo.add(testClassInfo);
        }

        return listTestClassInfo;
    }
}