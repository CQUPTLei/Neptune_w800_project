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
 * BaseLine
 */
public class BaseLine {
    private BaseLineParser baseLineParser = new BaseLineParser();

    /**
     * BaseLine construction
     *
     * @param filePath config FilePath
     */
    public BaseLine(String filePath) {
        baseLineParser = new BaseLineParser();
        if (baseLineParser != null) {
            baseLineParser.loadConfig(filePath);
        }
    }

    public BaseLineParser getBaseLineParser() {
        return baseLineParser;
    }

    @Override
    public String toString() {
        return "BaseLine [baseLineParser=" + baseLineParser + "]";
    }
}

