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

package ohos.junit.distribute;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 * common method
 */
public class CommonUtil {
    private static final String SEP_CHAR = ": ";

    private static final String LOG_UTIL_NAME = "CommonUtil";
    private String rootPath = "";

    /**
     * singleton
     */
    private static class SingletonHolder {
        private static final CommonUtil INSTANCE = new CommonUtil();
    }

    private CommonUtil() {
    }

    /**
     * util instance
     *
     * @return singleton
     */
    public static CommonUtil getInstance() {
        return SingletonHolder.INSTANCE;
    }

    /**
     * get current workspace path
     *
     * @apiNote get current workspace path
     *
     * @return current workspace path
     */
    public String getCurrentPath() {
        if (rootPath.isEmpty()) {
            File curFile = new File("");
            try {
                rootPath = curFile.getCanonicalPath();
            } catch (IOException e) {
                CommonUtil.getInstance().info(LOG_UTIL_NAME, "get rootpath failed." + e.getMessage());
            }
        }
        return rootPath;
    }

    /**
     * whether is file and exist
     *
     * @apiNote whether file is and exists
     *
     * @param fileObj input object
     * @return file and exist-true,else-false
     */
    public boolean isFileAndExist(File fileObj) {
        return fileObj.isFile() && fileObj.exists();
    }

    /**
     * whether string is null or empty
     *
     * @apiNote whether string is null or empty
     *
     * @param input input string
     * @return string is null or empty-true,else-false
     */
    public boolean isNullOrEmpty(String input) {
        if (input == null || input.isEmpty()) {
            return true;
        }
        return false;
    }

    /**
     * start thread
     *
     * @apiNote start thread
     *
     * @param runnable thread
     */
    public void startThread(Runnable runnable) {
        Thread agentThread = new Thread(runnable);
        agentThread.currentThread().setName("test_agent");
        agentThread.start();
        agentThread.setUncaughtExceptionHandler((Thread thread, Throwable exception) -> {
            System.out.println(thread.getName() + "_" + exception.getMessage());
            CommonUtil.getInstance().info(
                LOG_UTIL_NAME, thread.getName() + "_" + exception.getMessage());
        });
    }

    /**
     * serialize object to string,only ISO-8859-1 is available
     *
     * @param object source object
     * @return serialize string
     * @throws IOException serial exception
     */
    public String serializeToStr(Object object) throws IOException {
        ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
        ObjectOutputStream objOutput = new ObjectOutputStream(byteOutput);
        objOutput.writeObject(object);
        String returnStr = byteOutput.toString("ISO-8859-1");
        byteOutput.close();
        objOutput.close();
        return returnStr;
    }

    /**
     * deserialize string to object, only ISO-8859-1 is available
     *
     * @param inputStr input string
     * @return target object
     * @throws IOException serial exception
     */
    public Object deserializeToObj(String inputStr) throws IOException {
        if (isNullOrEmpty(inputStr)) {
            return "";
        }
        ByteArrayInputStream byteInput = new ByteArrayInputStream(inputStr.getBytes("ISO-8859-1"));
        ObjectInputStream objInput = new ObjectInputStream(byteInput);
        Object object = null;
        try {
            object = objInput.readObject();
        } catch (ClassNotFoundException e) {
            CommonUtil.getInstance().info(LOG_UTIL_NAME, "ClassNotFoundException." + e.getMessage());
        }
        byteInput.close();
        objInput.close();
        return object;
    }

    /**
     * print info level log
     *
     * @param name current class or function name
     * @param content content string
     */
    public final void info(String name, String content) {
        System.out.println(name + SEP_CHAR + content);
    }
}
