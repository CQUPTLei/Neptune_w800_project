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
import ohos.unittest.runner.record.ReportGenerater;
import ohos.unittest.runner.record.TestClassInfo;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

/**
 * Test Runner
 */
public abstract class TestRunner {
    /**
     * mini test parameter count
     */
    private static final int MIN_ARGS_COUNT = 3;

    /**
     * Test class keywords for recognition
     */
    private static final String CASEFILE_KEYWORDS = "Test";

    /**
     * Test result output dir path
     */
    private static final String OUTPUT_DIR = "OUTPUT_DIR";

    /**
     * Test result output file path
     */
    private static final String OUTPUT_FILE = "OUTPUT_FILE";

    /**
     * Jacoco flag
     */
    private static final String COVERAGE_FLAG = "COVERAGE_FLAG";

    /**
     * All test class for current execution
     */
    private static final String TEST_CLASS = "TEST_CLASS";

    /**
     * Specially specified test class for current execution
     */
    private static final String EXEC_CLASS = "EXEC_CLASS";

    /**
     * Specially specified test method for current execution
     */
    private static final String EXEC_METHOD = "EXEC_METHOD";

    /**
     * Specially specified test case level for current execution
     */
    private static final String EXEC_LEVEL = "EXEC_LEVEL";

    /**
     * execute Class name
     */
    private String execClassName = "";

    /**
     * execute Class Method name
     */
    private String execMethodName = "";

    /**
     * execute Class Method name
     */
    private ArrayList<Level> execCaseLevel = new ArrayList<>();

    /**
     * Filtered class for unittest test
     */
    private ArrayList<String> filteredClass = new ArrayList<>();

    /**
     * execute Class Method name
     */
    private String exportPath = "/data/test";

    public String getExecClassName() {
        return execClassName;
    }

    public String getExecMethodName() {
        return execMethodName;
    }

    public ArrayList<Level> getExecCaseLevel() {
        return execCaseLevel;
    }

    /**
     * ZRunner Constructor
     */
    public TestRunner() {
        this.execClassName = "";
        this.execMethodName = "";
        this.filteredClass = new ArrayList<>();
        this.filteredClass.add("ohos.unittest.R");
        this.filteredClass.add("ohos.unittest.App");
        this.filteredClass.add("ohos.unittest.AppContext");
        this.filteredClass.add("ohos.unittest.MainAbility");
        this.filteredClass.add("ohos.unittest.log.LogUtil");
        this.filteredClass.add("ohos.unittest.TestApplication");
        this.filteredClass.add("ohos.unittest.MainAbilityShellActivity");
    }

    private void printCorrectTips() {
        Logger.info("###Run Test Error, please input corrent args" + System.lineSeparator());
        Logger.info("#########Tips: Corrent Info needed" + System.lineSeparator());
        Logger.info("############First arg is folder path" + System.lineSeparator());
        Logger.info("############Second arg is file name" + System.lineSeparator());
        Logger.info("############Others is test class full path and parameters" + System.lineSeparator());
    }

    /**
     * Prepare Folder to saving test resutls
     *
     * @param strFolderPath FolderPath
     * @param strFileName FileName
     * @return file path
     */
    private String prepareResutlFolder(String strFolderPath, String strFileName) {
        String strFilePath = "";
        if (strFolderPath != null && !strFolderPath.isEmpty() && strFileName != null && !strFileName.isEmpty()) {
            File folder = new File(strFolderPath);
            if (!folder.exists()) {
                folder.mkdirs();
            }

            strFilePath = strFolderPath + strFileName;
            File file = new File(strFilePath);
            if (file.exists()) {
                file.delete();
            }
            return strFilePath;
        }

        return strFilePath;
    }

    /**
     * Run test from class name
     *
     * @param lstClassName list ClassName
     * @return all TestClassInfo list
     * @throws IOException
     */
    private List<TestClassInfo> runClassByName(List<String> lstClassName) throws IOException {
        List<TestClassInfo> listRecorder = new ArrayList<>();
        for (String strName : lstClassName) {
            String strClassName = strName.trim();
            if (strClassName.isEmpty()) {
                continue;
            }
            listRecorder.addAll(runClassByNameImp(strClassName));
        }
        return listRecorder;
    }

    /**
     * execute single class
     *
     * @param lstClassName class name needed to execute
     * @return all TestClassInfo list
     * @throws IOException
     */
    public abstract List<TestClassInfo> runClassByNameImp(String lstClassName) throws IOException;

    /**
     * Get test class name by parser Test Class parameter
     *
     * @param lstTestClassArg ClassArg
     * @return class name list
     */
    private List<String> getlstClassName(List<String> lstTestClassArg) {
        List<String> lstRet = new ArrayList<>();
        if (lstTestClassArg == null || lstTestClassArg.isEmpty()) {
            return lstRet;
        }

        for (String strFullPath : lstTestClassArg) {
            // get java class name
            List<String> listPre = Arrays.asList(strFullPath.split(","));

            for (String strTmpValue : listPre) {
                String value = strTmpValue.trim();
                if (value == null || value.isEmpty() || filteredClass.contains(value)) {
                    continue;
                }

                String strClass = value;
                int lastDotPos = strClass.lastIndexOf(".");
                if (lastDotPos > 0) {
                    String className = strClass.substring(lastDotPos + 1, strClass.length());
                    if (className.length() > CASEFILE_KEYWORDS.length()) {
                        String strSubFix =
                            className.substring(className.length() - CASEFILE_KEYWORDS.length(), className.length());
                        if (strSubFix.equalsIgnoreCase(CASEFILE_KEYWORDS)) {
                            lstRet.add(strClass);
                        }
                    }
                }
            }
        }
        return lstRet;
    }

    /**
     * Convert test parameter to map format
     *
     * @param args test parameter
     * @return parameter as map format
     */
    private Map<String, String> convertToMap(String[] args) {
        Map<String, String> mapArgs = new HashMap<>();
        int length = args.length;
        for (int index = 0; index < length; index++) {
            String strArg = args[index].trim();
            int equalPos = strArg.indexOf("=");
            if (equalPos > 0) {
                String strKey = strArg.substring(0, equalPos).trim();
                String strValue = "";
                if (strArg.length() > equalPos) {
                    strValue = strArg.substring(equalPos + 1, strArg.length()).trim();
                }
                mapArgs.put(strKey, strValue);
            }
        }
        return mapArgs;
    }

    /**
     * Run test by parameter
     *
     * @param args test parameter
     */
    public void run(String[] args) {
        int length = args.length;
        Map<String, String> mapArgs = convertToMap(args);
        String exportPathParam = mapArgs.get(OUTPUT_DIR);
        String resultFileName = mapArgs.get(OUTPUT_FILE);
        if (exportPathParam == null || exportPathParam.isEmpty()) {
            exportPath = "/data/test";
        } else {
            exportPath = exportPathParam;
        }

        String resultFilePathLog = prepareResutlFolder(exportPath, resultFileName + ".log");
        Logger.createLogFile(resultFilePathLog);
        if (length < MIN_ARGS_COUNT) {
            printCorrectTips();
            return;
        }

        String resultFilePath = prepareResutlFolder(exportPath, resultFileName + ".xml");
        if (resultFilePath.isEmpty()) {
            Logger.info("prepare Resutl Folder error!");
            return;
        }

        List<String> lstTestClassArg = new ArrayList<>();
        String strTestClassArg = mapArgs.get(TEST_CLASS);
        if (strTestClassArg != null && !strTestClassArg.isEmpty()) {
            lstTestClassArg.add(strTestClassArg);
        }

        initExecParemater(mapArgs);

        runTestCase(resultFilePath, lstTestClassArg);

        String coverageFlag = mapArgs.get(COVERAGE_FLAG);
        if (coverageFlag != null && !coverageFlag.isEmpty() && coverageFlag.equalsIgnoreCase("1")) {
            String execFilePath = exportPath + "jacoco.exec";
            if (!genJacocoExecData(execFilePath)) {
                Logger.info("Generate jacoco execution data failed, "
                    + "Please confirm that you have restarted the device after upgrading the system.");
            }
        }

        String testRunStatusFile = prepareResutlFolder(exportPath, "jtest_status.txt");
        List<String> fileData = new ArrayList<>();
        fileData.add("Java testcase executed finished");
        LogFile.createFileAndWriteData(testRunStatusFile, fileData);
    }

    /**
     * Parser test parameter
     *
     * @param mapArgs test parameter
     */
    private void initExecParemater(Map<String, String> mapArgs) {
        if (mapArgs.isEmpty()) {
            return;
        }

        if (mapArgs.containsKey(EXEC_CLASS) && !mapArgs.get(EXEC_CLASS).isEmpty()) {
            execClassName = mapArgs.get(EXEC_CLASS);
        }

        if (mapArgs.containsKey(EXEC_METHOD) && !mapArgs.get(EXEC_METHOD).isEmpty()) {
            execMethodName = mapArgs.get(EXEC_METHOD);
        }

        String strExecCaseLevels = mapArgs.get(EXEC_LEVEL);
        if (strExecCaseLevels != null && !strExecCaseLevels.isEmpty()) {
            String levelPrefix = "Level";
            List<String> listCaseLevel = Arrays.asList(strExecCaseLevels.split(","));
            for (String strCaseLevel : listCaseLevel) {
                String caseLevel = strCaseLevel;
                int prefixPos = strCaseLevel.indexOf(levelPrefix);
                if (prefixPos < 0) {
                    caseLevel = strCaseLevel;
                } else {
                    caseLevel = strCaseLevel.substring(prefixPos + levelPrefix.length(), strCaseLevel.length());
                }

                if ("0".equals(caseLevel)) {
                    this.execCaseLevel.add(Level.LEVEL_0);
                }
                if ("1".equals(caseLevel)) {
                    this.execCaseLevel.add(Level.LEVEL_1);
                }
                if ("2".equals(caseLevel)) {
                    this.execCaseLevel.add(Level.LEVEL_2);
                }
                if ("3".equals(caseLevel)) {
                    this.execCaseLevel.add(Level.LEVEL_3);
                }
                if ("4".equals(caseLevel)) {
                    this.execCaseLevel.add(Level.LEVEL_4);
                }
            }
        }
    }

    /**
     * Test running and save results
     *
     * @param strFilePath FilePath
     * @param lstTestClassArg TestClassArg
     * @return true or false
     */
    private boolean runTestCase(String strFilePath, List<String> lstTestClassArg) {
        boolean isSuccess = false;
        List<String> lstTestClass = getlstClassName(lstTestClassArg);
        try {
            List<String> lstAllTestClass = new ArrayList<>();
            lstAllTestClass.addAll(lstTestClass);
            List<TestClassInfo> listTestClassInfo = runClassByName(lstAllTestClass);
            ReportGenerater reportGenerater = new ReportGenerater();
            reportGenerater.generateXmlReport(strFilePath, listTestClassInfo);
            return true;
        } catch (IOException e) {
            Logger.info("generateXmlReport Exception with message:" + e.getMessage());
        }
        return isSuccess;
    }

    /**
     * Generate java code coverage execution data, and save it in file
     *
     * @param java code coverage execution data File Path
     * @return true or false
     */
    private boolean genJacocoExecData(String execFilePath) {
        boolean isSuccess = false;
        OutputStream out = null;
        try {
            out = new FileOutputStream(execFilePath, false);
            isSuccess = saveJacocoExecData(out);
        } catch (FileNotFoundException e) {
            Logger.info("GenJacocoExecData FileNotFoundException with message:" + e.getMessage());
        } catch (ClassNotFoundException e) {
            Logger.info("GenJacocoExecData ClassNotFoundException with message:" + e.getMessage());
        } catch (NoSuchMethodException e) {
            Logger.info("GenJacocoExecData NoSuchMethodException with message:" + e.getMessage());
        } catch (IllegalAccessException e) {
            Logger.info("GenJacocoExecData IllegalAccessException with message:" + e.getMessage());
        } catch (InvocationTargetException e) {
            Logger.info("GenJacocoExecData InvocationTargetException with message:" + e.getMessage());
            Throwable throwable = e.getTargetException();
            Logger.info("GenJacocoExecData FileNotFoundException Throwable with message:" + throwable.getMessage());
        } catch (IOException e) {
            Logger.info("GenJacocoExecData IOException with message:" + e.getMessage());
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    Logger.info("GenJacocoExecData close IOException with message:" + e.getMessage());
                }
            }
        }
        return isSuccess;
    }

    /**
     * Save java code coverage execution Data to OutputStream
     *
     * @param out OutputStream
     * @return true or false
     * @throws ClassNotFoundException
     * @throws NoSuchMethodException
     * @throws IllegalAccessException
     * @throws InvocationTargetException
     * @throws IOException
     */
    private boolean saveJacocoExecData(OutputStream out) throws ClassNotFoundException, NoSuchMethodException,
        IllegalAccessException, InvocationTargetException, IOException {
        boolean isSuccess = false;
        Class<?> testClass = Class.forName("org.jacoco.agent.rt.RT");
        if (testClass == null) {
            Logger.info("saveJacocoExecData load class org.jacoco.agent.rt.RT failed");
            return isSuccess;
        }

        Method method = testClass.getMethod("getAgent");
        if (method == null) {
            Logger.info("saveJacocoExecData load method getAgent() from org.jacoco.agent.rt.RT failed");
            return isSuccess;
        }

        Object agent = method.invoke(null);
        if (agent == null) {
            Logger.info("saveJacocoExecData invoke method getAgent() of from org.jacoco.agent.rt.RT failed");
            return isSuccess;
        }

        if (out == null) {
            Logger.info("saveJacocoExecData OutputStream out is null");
            return isSuccess;
        }

        Method execMethod = agent.getClass().getMethod("getExecutionData", boolean.class);
        if (execMethod == null) {
            Logger.info("saveJacocoExecData getExecutionData is null");
            return isSuccess;
        }

        Object object = execMethod.invoke(agent, false);
        if (object == null) {
            Logger.info("saveJacocoExecData invoke getExecutionData is null");
            return isSuccess;
        }

        if (object instanceof byte[]) {
            out.write((byte[]) object);
            isSuccess = true;
        } else {
            Logger.info("saveJacocoExecData invoke getExecutionData result is not byte[]");
        }
        return isSuccess;
    }
}