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

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Map.Entry;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;


/**
 * BaseLineParser
 */
class BaseLineParser {
    private static final int DEFAULT_DOUBLE_VALUE = -1;

    private static final String BASELINE_KEY = "baseline";

    private static final String LASTVALUE_KEY = "lastvalue";

    private static final String FLOATRANGE_KEY = "floatrange";

    /**
     * case baseline value
     */
    private HashMap<String /* case name */, HashMap<String /* item key */, String /* item value */>> caseBaseLine =
        new HashMap<>();

    /**
     * baseline url
     */
    private String url = "";

    /**
     * baseline date
     */
    private String date = "";

    /**
     * isHasBaseLineCfg
     */
    private boolean isHasBaseLineCfg = false;

    /**
     * BaseLine constructor
     */
    BaseLineParser() {
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public Map<String /* case name */, HashMap<String /* item key */, String /* item value */>> getCaseBaseLine() {
        return caseBaseLine;
    }

    /**
     * isHasBaselineConfig
     *
     * @apiNote isHasBaselineConfig
     * @return boolean
     */
    public boolean isHasBaselineConfig() {
        return isHasBaseLineCfg;
    }

    /**
     * loadConfig
     *
     * @apiNote loadConfig
     * @param configFilePath configFilePath
     * @return boolean
     */
    public boolean loadConfig(String configFilePath) {
        boolean isSuccess = false;
        if (configFilePath == null || configFilePath.trim().isEmpty()) {
            Logger.info(String.format(Locale.ROOT, "[ ERROR    ] Performance config file path can't be empty"));
            return isSuccess;
        }

        File file = new File(configFilePath.trim());
        if (file.exists()) {
            try {
                Document document = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(file);
                parserBaseline(document.getFirstChild());
                isSuccess = true;
                isHasBaseLineCfg = true;
            } catch (SAXException e) {
                Logger.info(String.format(Locale.ROOT, "SAXException: %s", e.getMessage()));
            } catch (IOException e) {
                Logger.info(String.format(Locale.ROOT, "IOException: %s", e.getMessage()));
            } catch (ParserConfigurationException e) {
                Logger.info(String.format(Locale.ROOT, "ParserConfigurationException: %s", e.getMessage()));
            }
        } else {
            Logger.info(String.format(Locale.ROOT, "File:%s not exists", configFilePath));
        }
        showParsedBaseline();
        return isSuccess;
    }

    /**
     * parser xml document
     *
     * @apiNote parser
     * @param root root
     */
    public void parserBaseline(Node root) {
        NodeList childNodes = root.getChildNodes();
        String configLable = root.getNodeName();
        if ("configuration".equals(configLable)) {
            NamedNodeMap configLableAttribute = root.getAttributes();
            if (configLableAttribute != null) {
                for (int index = 0; index < configLableAttribute.getLength(); index++) {
                    Node attribute = configLableAttribute.item(index);
                    if ("date".equals(attribute.getNodeName())) {
                        this.date = attribute.getNodeValue();
                    }

                    if ("url".equals(attribute.getNodeName())) {
                        this.url = attribute.getNodeValue();
                    }
                }
            }
        }

        if (childNodes == null) {
            return;
        }

        for (int index = 0; index < childNodes.getLength(); index++) {
            Node caseItem = childNodes.item(index);
            String caseLable = caseItem.getNodeName();
            if (!"item".equals(caseLable)) {
                continue;
            }
            String testCaseName = null;
            HashMap<String, String> caseValue = new HashMap<>();
            NamedNodeMap caseLableAttribute = caseItem.getAttributes();
            for (int id = 0; id < caseLableAttribute.getLength(); id++) {
                Node attribute = caseLableAttribute.item(id);
                if ("name".equals(attribute.getNodeName())) {
                    testCaseName = attribute.getNodeValue();
                }
                caseValue.put(attribute.getNodeName(), attribute.getNodeValue());
            }

            if (testCaseName == null || testCaseName.isEmpty()) {
                continue;
            }
            caseBaseLine.put(testCaseName, caseValue);
        }
    }

    /**
     * Show Parsed Baseline
     *
     * @apiNote show config info
     */
    public void showParsedBaseline() {
        Logger.info(String.format(Locale.ROOT, "baseline info:"));
        Logger.info(String.format(Locale.ROOT, "  update date : %s", getDate()));
        Logger.info(String.format(Locale.ROOT, "  data from db: %s", getUrl()));
        Logger.info(String.format(Locale.ROOT, "  total items : %s", String.valueOf(getCount())));
        for (Entry<String, HashMap<String, String>> entry : caseBaseLine.entrySet()) {
            Logger.info(String.format(Locale.ROOT, "case:%s %s:%s %s:%s %s:%s", entry.getKey(), BASELINE_KEY,
                entry.getValue().get(BASELINE_KEY), LASTVALUE_KEY, entry.getValue().get(LASTVALUE_KEY), FLOATRANGE_KEY,
                entry.getValue().get(FLOATRANGE_KEY)));
        }
    }

    /**
     * getCount
     *
     * @apiNote getCount
     * @return int
     */
    public int getCount() {
        if (caseBaseLine != null) {
            return caseBaseLine.size();
        } else {
            return 0;
        }
    }

    /**
     * isHasBaseLineValue
     *
     * @apiNote isHasBaseLineValue
     * @param caseName test case name
     * @return boolean
     */
    public boolean isHasBaseLineValue(String caseName) {
        return isContainCaseBaseValue(caseName, BASELINE_KEY);
    }

    /**
     * isHasLastValue
     *
     * @apiNote isHasLastValue
     * @param caseName test case name
     * @return boolean
     */
    public boolean isHasLastValue(String caseName) {
        return isContainCaseBaseValue(caseName, LASTVALUE_KEY);
    }

    /**
     * isHasFloatRange
     *
     * @apiNote isHasFloatRange
     * @param caseName test case name
     * @return boolean
     */
    public boolean isHasFloatRange(String caseName) {
        return isContainCaseBaseValue(caseName, FLOATRANGE_KEY);
    }

    /**
     * getBaseLineValue
     *
     * @apiNote getBaseLineValue
     * @param caseName test case name
     * @return double
     */
    public double getBaseLineValue(String caseName) {
        if (caseBaseLine.containsKey(caseName)) {
            return convertStr2Double(caseBaseLine.get(caseName).get(BASELINE_KEY));
        } else {
            return DEFAULT_DOUBLE_VALUE;
        }
    }

    /**
     * getLastValue
     *
     * @apiNote getLastValue
     * @param caseName test case name
     * @return double
     */
    public double getLastValue(String caseName) {
        if (caseBaseLine.containsKey(caseName)) {
            return convertStr2Double(caseBaseLine.get(caseName).get(LASTVALUE_KEY));
        } else {
            return DEFAULT_DOUBLE_VALUE;
        }
    }

    /**
     * getFloatRange
     *
     * @apiNote getFloatRange
     * @param caseName test case name
     * @return double
     */
    public double getFloatRange(String caseName) {
        if (caseBaseLine.containsKey(caseName)) {
            return convertStr2Double(caseBaseLine.get(caseName).get(FLOATRANGE_KEY));
        } else {
            return DEFAULT_DOUBLE_VALUE;
        }
    }

    /**
     * isContainCaseBaseValue
     *
     * @apiNote isContainCaseBaseValue
     * @param caseName test case name
     * @param itemName item name
     * @return boolean
     */
    private boolean isContainCaseBaseValue(String caseName, String itemName) {
        if (caseName == null || caseName.isEmpty()) {
            return false;
        }

        if (caseBaseLine == null || caseBaseLine.isEmpty()) {
            return false;
        }

        HashMap<String, String> caseItem = caseBaseLine.get(caseName);
        if (caseItem == null) {
            return false;
        }
        String value = caseItem.get(itemName);
        if (value == null || value.isEmpty() || convertStr2Double(value) <= 0) {
            return false;
        }

        return true;
    }

    private double convertStr2Double(String str) {
        if (str.isEmpty()) {
            return 0.0;
        }
        return Double.valueOf(str);
    }

    @Override
    public String toString() {
        return "BaseLineParser [caseBaseLine=" + caseBaseLine + ", url=" + url + ", date=" + date
            + ", isHasBaseLineCfg=" + isHasBaseLineCfg + "]";
    }
}
