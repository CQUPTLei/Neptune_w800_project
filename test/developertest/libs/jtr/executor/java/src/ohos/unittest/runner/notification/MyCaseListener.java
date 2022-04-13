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

import org.junit.runner.Description;
import java.util.ArrayList;

import ohos.unittest.listener.CaseListener;
import ohos.unittest.CaseLevel;
import ohos.unittest.Level;


/**
 * User defined Junit CaseListener
 */
public class MyCaseListener extends CaseListener {
    /**
     * Default test class or method flag, it means "all"
     */
    private static final String DEFAULT = "*";

    /**
     * execute Class name
     */
    private String execClassName = "";

    /**
     * execute Class Method name
     */
    private String execMethodName = "";

    /**
     * execute Class level list
     */
    private ArrayList<Level> execCaseLevel = new ArrayList<>();

    /**
     * ZRunCase constructor
     */
    public MyCaseListener() {
        this.execClassName = "";
        this.execMethodName = "";
    }

    public String getExecClassName() {
        return execClassName;
    }

    public void setExecClassName(String execClassName) {
        this.execClassName = execClassName;
    }

    public String getExecMethodName() {
        return execMethodName;
    }

    public void setExecMethodName(String execMethodName) {
        this.execMethodName = execMethodName;
    }

    public ArrayList<Level> getExecCaseLevel() {
        return execCaseLevel;
    }

    public void setExecCaseLevel(ArrayList<Level> execCaseLevel) {
        this.execCaseLevel = execCaseLevel;
    }

    @Override
    public boolean isContinue(Description description) {
        String strClassName = description.getClassName();
        String strMethodName = description.getMethodName();

        if (execClassName != null && !execClassName.isEmpty() && !execClassName.equals(DEFAULT)
            && !execClassName.equals(strClassName)) {
            return false;
        }

        if (execMethodName != null && !execMethodName.isEmpty() && !execMethodName.equals(DEFAULT)
            && !execMethodName.equals(strMethodName)) {
            return false;
        }

        if (this.execCaseLevel != null && !this.execCaseLevel.isEmpty()) {
            CaseLevel caseLevel = description.getAnnotation(CaseLevel.class);
            if (caseLevel != null && this.execCaseLevel.contains(caseLevel.level())) {
                return true;
            } else {
                return false;
            }
        }

        return true;
    }

    @Override
    public String toString() {
        return "ZCaseListener [execClassName=" + execClassName + ", execMethodName=" + execMethodName
            + ", execCaseLevel=" + execCaseLevel + "]";
    }
}
