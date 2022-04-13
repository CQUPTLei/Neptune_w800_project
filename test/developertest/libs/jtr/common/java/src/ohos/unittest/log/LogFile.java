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
package ohos.unittest.log;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import java.util.List;


/**
 * Log File
 */
public class LogFile {
    /**
     * LogFile instance
     */
    private static LogFile instance = null;

    /**
     * Log file path
     */
    private String logFilePath;

    /**
     * LogFile Constructor
     */
    private LogFile() {
    }

    /**
     * get LogFile single Instance
     *
     * @return LogFile Instance
     */
    public static LogFile getInstance() {
        if (instance == null) {
            instance = new LogFile();
        }

        return instance;
    }

    /**
     * Print log information in file
     *
     * @param content log content
     */
    public void println(String content) {
        if (content == null) {
            return;
        }

        writeFile(content);
    }

    /**
     * Create log File
     *
     * @param filePath log file Path
     */
    public void createLogFile(String filePath) {
        File file = new File(filePath);

        if (file.exists() && !file.isDirectory()) {
            file.delete();
        }

        try {
            file.createNewFile();
            this.logFilePath = filePath;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public String getLogFilePath() {
        return this.logFilePath;
    }

    public void setLogFilePath(String logFilePath) {
        this.logFilePath = logFilePath;
    }

    /**
     * Create File and write content in it
     *
     * @param filePath file Path
     * @param fileData file Data
     */
    public static void createFileAndWriteData(String filePath,
        List<String> fileData) {
        FileWriter writer = null;

        try {
            File file = new File(filePath);

            if (file.exists() && !file.isDirectory()) {
                file.delete();
            }

            file.createNewFile();
            writer = new FileWriter(file);

            for (String content : fileData) {
                writer.write(content + System.lineSeparator());
            }

            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Write File with content
     *
     * @param content log information
     */
    private void writeFile(String content) {
        FileWriter writer = null;

        try {
            if ((this.logFilePath == null) || "".equals(this.logFilePath)) {
                return;
            }

            File file = new File(this.logFilePath);

            if (file.isDirectory()) {
                return;
            }

            if (!file.exists()) {
                file.createNewFile();
            }

            writer = new FileWriter(file, true);
            writer.write(content + System.lineSeparator());
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
