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

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Map.Entry;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import ohos.unittest.log.Logger;
import ohos.unittest.perf.VerifySingleton;

/**
 * Record test result as xml format
 */
public class ReportGenerater {
    /**
     * constant data:1000.0
     */
    private static final double ONE_THOUSAND = 1000.0;

    /**
     * Test result key
     */
    private static final String RESULT = "result";

    /**
     * Test class name key
     */
    private static final String CLASSNAME = "classname";

    /**
     * Test result key
     */
    private static final String NAME = "name";

    /**
     * Test execution time key
     */
    private static final String TIME = "time";

    /**
     * File format
     */
    private static final String UTF_8 = "UTF-8";

    /**
     * ReportGenerater Constructor
     */
    public ReportGenerater() {
    }

    /**
     * Write data to document
     *
     * @param document xml document object
     * @param filePath xml file full path
     */
    private void writeXmlFile(Document document, String filePath) {
        if (!isValidate(document, filePath)) {
            return;
        }

        TransformerFactory transFactory = TransformerFactory.newInstance();
        ByteArrayOutputStream bytes = null;
        try {
            Transformer transformer = transFactory.newTransformer();
            if (transformer == null) {
                Logger.info("transformer object is null");
                return;
            }
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty(OutputKeys.ENCODING, UTF_8);
            transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");

            DOMSource source = new DOMSource();
            source.setNode(document);
            bytes = new ByteArrayOutputStream();
            StreamResult result = new StreamResult(bytes);
            transformer.transform(source, result);
            String xmlStr = bytes.toString(UTF_8);
            writeDataInFile(filePath, xmlStr);
        } catch (IOException e) {
            Logger.info("writeXmlFile IOException with message:" + e.getMessage());
        } catch (TransformerConfigurationException e) {
            Logger.info("writeXmlFile TransformerConfigurationException with message:" + e.getMessage());
        } catch (TransformerException e) {
            Logger.info("writeXmlFile TransformerException with message:" + e.getMessage());
        } finally {
            try {
                bytes.close();
            } catch (IOException e) {
                Logger.info("close the bytes of writeXmlFile:" + e.getMessage());
            }
        }
    }

    /**
     * Determine if Document and filePath is valid
     *
     * @param document XML document object
     * @param filePath xml filePath
     * @return true or false
     */
    private boolean isValidate(Document document, String filePath) {
        boolean isValidate = false;
        if (document == null) {
            Logger.info("Document object is null");
            return isValidate;
        }

        if (filePath == null) {
            Logger.info("filePath is null");
            return isValidate;
        }

        String parent = new File(filePath).getParent();
        if (parent == null) {
            Logger.info("filePath parent not exists");
            return isValidate;
        }

        File dirPath = new File(parent);
        if (!dirPath.exists()) {
            dirPath.mkdirs();
        }

        return true;
    }

    /**
     * Write data in file
     *
     * @param filePath file full path
     * @param fileData file data
     */
    private void writeDataInFile(String filePath, String fileData) {
        OutputStream fos = null;
        OutputStreamWriter writer = null;
        try {
            fos = new FileOutputStream(filePath);
            writer = new OutputStreamWriter(fos, UTF_8);
            writer.write(fileData);
        } catch (FileNotFoundException e1) {
            Logger.info("writeDataInFile FileNotFoundException with message:" + e1.getMessage());
        } catch (UnsupportedEncodingException e1) {
            Logger.info("writeDataInFile UnsupportedEncodingException with message:" + e1.getMessage());
        } catch (IOException e) {
            Logger.info("writeDataInFile IOException with message:" + e.getMessage());
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    Logger.info("writeDataInFile IOException1 with message:" + e.getMessage());
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    Logger.info("writeDataInFile IOException2 with message:" + e.getMessage());
                }
            }
        }
    }

    private String convertMillisecond2Second(long time) {
        double retSecond = 0;
        retSecond = time / ONE_THOUSAND;
        return String.format(Locale.ROOT, "%.3f", retSecond);
    }

    /**
     * Create XML Document by data list
     *
     * @param listReportData TestClassInfo list
     * @return XML Document object
     * @throws ParserConfigurationException
     */
    private Document createXmlDocument(List<TestClassInfo> listReportData) throws ParserConfigurationException {
        Document doc = initXmlDocument();
        if (doc == null) {
            return doc;
        }

        // Add root element
        addTestclassesExecInfoInDocument(listReportData, doc);
        return doc;
    }

    /**
     * addTestsuitExecInfo2Doc
     *
     * @param listReportData TestClassInfo execution list
     * @param doc xml Document object
     */
    private void addTestclassesExecInfoInDocument(List<TestClassInfo> listReportData, Document doc) {
        Element testsuites = doc.createElement("testsuites");
        if (testsuites == null) {
            Logger.info("Create testsuites element error");
            return;
        }
        doc.appendChild(testsuites);
        int allRunCount = 0;
        int allIgnoreCount = 0;
        int allFailureCount = 0;
        long allRunTime = 0L;
        String strTimestamp = "";
        for (TestClassInfo testClassInfo : listReportData) {
            // Add sub element
            Element testsuite = doc.createElement("testsuite");
            if (testsuite == null) {
                Logger.info("Create testsuite element error");
                continue;
            }
            testsuites.appendChild(testsuite);
            copyTestsuiteExecutedInfo(testClassInfo, testsuite);

            if (strTimestamp.isEmpty()) {
                strTimestamp = testClassInfo.getStartTime();
            }

            allRunTime += testClassInfo.getRunTime();
            allRunCount += testClassInfo.getRunCount();
            allFailureCount += testClassInfo.getFailureCount();
            allIgnoreCount += testClassInfo.getIgnoreCount();

            List<TestCaseInfo> lstTestCaseInfo = testClassInfo.getAllRanTestCaseInfo();
            if (lstTestCaseInfo == null || lstTestCaseInfo.isEmpty()) {
                Logger.info("Executed case is empty");
                continue;
            }

            addTestCaseElement2TestsSuite(doc, testsuite, lstTestCaseInfo);
        }

        testsuites.setAttribute("tests", String.valueOf(allRunCount));
        testsuites.setAttribute("failures", String.valueOf(allFailureCount));
        testsuites.setAttribute("ignores", String.valueOf(allIgnoreCount));

        testsuites.setAttribute("disabled", String.valueOf(0));
        testsuites.setAttribute("errors", String.valueOf(0));

        testsuites.setAttribute("timestamp", strTimestamp);
        testsuites.setAttribute(TIME, String.valueOf(convertMillisecond2Second(allRunTime)));
        testsuites.setAttribute(NAME, "");
    }

    /**
     * copy TestSuite Executed Information
     *
     * @param testClassInfo source testClassInfo
     * @param testsuite destination test suite
     */
    private void copyTestsuiteExecutedInfo(TestClassInfo testClassInfo, Element testsuite) {
        if (testClassInfo == null) {
            Logger.info("testClassInfo testClassInfo is null");
            return;
        }

        if (testsuite == null) {
            Logger.info("testsuite is null");
            return;
        }

        testsuite.setAttribute(NAME, testClassInfo.getClassName());
        int runCount = testClassInfo.getRunCount();
        int failureCount = testClassInfo.getFailureCount();
        int ignoreCount = testClassInfo.getIgnoreCount();

        testsuite.setAttribute("tests", String.valueOf(runCount));
        testsuite.setAttribute("failures", String.valueOf(failureCount));
        testsuite.setAttribute("ignores", String.valueOf(ignoreCount));

        testsuite.setAttribute("disabled", String.valueOf(0));
        testsuite.setAttribute("errors", String.valueOf(0));

        long runTime = testClassInfo.getRunTime();
        testsuite.setAttribute(TIME, String.valueOf(convertMillisecond2Second(runTime)));
        testsuite.setAttribute(RESULT, String.valueOf(testClassInfo.isSuccess()));
        if (!testClassInfo.isSuccess()) {
            testsuite.setAttribute("unavailable", String.valueOf(1));
            testsuite.setAttribute("message", String.valueOf(testClassInfo.getErrorMessage()));
        }
    }

    /**
     * Init Xml Document
     *
     * @return xml Document
     * @throws ParserConfigurationException
     */
    private Document initXmlDocument() throws ParserConfigurationException {
        // Instance DocumentBuilderFactory
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        if (factory == null) {
            Logger.info("DocumentBuilderFactory.newInstance error");
            return null;
        }

        factory.setNamespaceAware(true);
        DocumentBuilder documentBuilder = factory.newDocumentBuilder();

        // Create Document
        Document doc = documentBuilder.newDocument();
        if (doc == null) {
            Logger.info("newDocument error");
            return null;
        }

        return doc;
    }

    /**
     * Add TestCase Element to TestsSuite Element for xml Document
     *
     * @param doc XML Document
     * @param testsuite testsuite Element
     * @param lstTestCaseInfo TestCaseInfo list
     */
    private void addTestCaseElement2TestsSuite(Document doc, Element testsuite, List<TestCaseInfo> lstTestCaseInfo) {
        for (TestCaseInfo testCaseInfo : lstTestCaseInfo) {
            // Add sub element
            Element testcase = doc.createElement("testcase");
            if (testcase == null) {
                Logger.info("Create testcase element error");
                continue;
            }

            testcase.setAttribute(NAME, testCaseInfo.getName());
            if (testCaseInfo.isIgnored()) {
                testcase.setAttribute("status", "notrun");
            } else {
                testcase.setAttribute("status", "run");
            }

            testcase.setAttribute(TIME, String.valueOf(convertMillisecond2Second(testCaseInfo.getTotalTime())));
            testcase.setAttribute(CLASSNAME, testCaseInfo.getClassName());
            testcase.setAttribute(RESULT, String.valueOf(testCaseInfo.isSuccess()));
            if (!testCaseInfo.isSuccess()) {
                Element failureElemment = doc.createElement("failure");
                if (testCaseInfo.getErrorMsg() != null && !testCaseInfo.getErrorMsg().isEmpty()) {
                    failureElemment.setAttribute("message", testCaseInfo.getErrorMsg());
                } else if (testCaseInfo.getTraceInfo() != null && !testCaseInfo.getTraceInfo().isEmpty()) {
                    int index = testCaseInfo.getTraceInfo().indexOf("org.junit.runners.model.FrameworkMethod");
                    String message = testCaseInfo.getTraceInfo();
                    if (index > 0) {
                        message = testCaseInfo.getTraceInfo().substring(0, index) + " ......";
                    }
                    failureElemment.setAttribute("message",  message);
                } else {
                    failureElemment.setAttribute("message", "");
                }

                failureElemment.setAttribute("type", "");
                failureElemment.setTextContent(testCaseInfo.getErrorMsg());
                testcase.appendChild(failureElemment);
            }

            addPerfTestResult(doc, testcase, testCaseInfo);

            testcase.setAttribute("level", String.valueOf(testCaseInfo.getLevel()));
            testsuite.appendChild(testcase);
        }
    }

    private void addPerfTestResult(Document doc, Element testcase, TestCaseInfo testCaseInfo) {
        if (doc == null || testcase == null || testCaseInfo == null) {
            return;
        }

        boolean isPerfTestcase = false;
        Map<String, String> perfResult = null;
        String className = testCaseInfo.getClassName();
        String caseName = testCaseInfo.getName();
        if (VerifySingleton.getInstance().isPerfTestcase(className, caseName)) {
            isPerfTestcase = true;
            perfResult = VerifySingleton.getInstance().getPerfTestResult(className,
            caseName);
        } else {
            isPerfTestcase = false;
            perfResult = null;
        }

        if (isPerfTestcase && perfResult != null) {
            Element propertiesElemment = doc.createElement("properties");
            for (Entry<String, String> entry : perfResult.entrySet()) {
                    Element property = doc.createElement("property");
                    property.setAttribute("name", entry.getKey());
                    property.setAttribute("value", entry.getValue());
                    propertiesElemment.appendChild(property);
            }

            Element propertyClass = doc.createElement("property");
            propertyClass.setAttribute("name", "test_class");
            propertyClass.setAttribute("value", testCaseInfo.getTestTargetClass());
            propertiesElemment.appendChild(propertyClass);

            Element propertyMethod = doc.createElement("property");
            propertyMethod.setAttribute("name", "test_interface");
            propertyMethod.setAttribute("value", testCaseInfo.getTestTargetMethod());
            propertiesElemment.appendChild(propertyMethod);

            testcase.appendChild(propertiesElemment);
        }
    }

    /**
     * Save testing results to XML file
     *
     * @param strReportPath xml test report file path
     * @param listReportData test results
     * @return true or false
     */
    public boolean generateXmlReport(String strReportPath, List<TestClassInfo> listReportData) {
        try {
            Document document = createXmlDocument(listReportData);
            writeXmlFile(document, strReportPath);
        } catch (ParserConfigurationException e) {
            Logger.info("generateXmlReport Exception with message:" + e.getMessage());
            return false;
        }
        return true;
    }
}

/**
 * XmlConstants
 */
final class XmlConstants {
    /**
     * XML ACCESS_EXTERNAL_DTD
     */
    public static final String ACCESS_EXTERNAL_DTD = "http://javax.xml.XMLConstants/property/accessExternalDTD";

    /**
     * XML ACCESS_EXTERNAL_STYLESHEET
     */
    public static final String ACCESS_EXTERNAL_STYLESHEET =
        "http://javax.xml.XMLConstants/property/accessExternalStylesheet";

    private XmlConstants() {
    }
}
