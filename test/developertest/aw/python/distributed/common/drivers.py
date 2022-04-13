#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
import time
import json
import shutil
import zipfile
import tempfile
from abc import ABCMeta
from abc import abstractmethod

from core.config.resource_manager import ResourceManager


##############################################################################
##############################################################################

DEVICE_TEST_PATH = "/%s/%s/" % ("data", "test")
INSTRUMENT_HAP_FILE_SUFFIX = '_ad.hap'
TIME_OUT = 900 * 1000


class ZunitConst(object):
    z_unit_app = "ohos.unittest.App"
    output_dir = "OUTPUT_DIR="
    output_file = "OUTPUT_FILE="
    test_class = "TEST_CLASS="
    exec_class = "EXEC_CLASS="
    exec_method = "EXEC_METHOD="
    exec_level = "EXEC_LEVEL="
    jtest_status_filename = "jtest_status.txt"
    remote_command_dir = "commandtmp"


def get_level_para_string(level_string):
    level_list = list(set(level_string.split(",")))
    level_para_string = ""
    for item in level_list:
        if not item.isdigit():
            continue
        item = item.strip(" ")
        level_para_string += ("Level%s," % item)
    level_para_string = level_para_string.strip(",")
    return level_para_string


def get_java_test_para(testcase, testlevel):
    exec_class = "*"
    exec_method = "*"
    exec_level = ""

    if "" != testcase and "" == testlevel:
        pos = testcase.rfind(".")
        if pos != -1:
            exec_class = testcase[0:pos]
            exec_method = testcase[pos + 1:]
            exec_level = ""
        else:
            exec_class = "*"
            exec_method = testcase
            exec_level = ""
    elif "" == testcase and "" != testlevel:
        exec_class = "*"
        exec_method = "*"
        exec_level = get_level_para_string(testlevel)

    return exec_class, exec_method, exec_level


def get_execute_java_test_files(suite_file):
    java_test_file = ""
    test_info_file = suite_file[:suite_file.rfind(".")] + ".info"
    if not os.path.exists(test_info_file):
        return java_test_file
    try:
        with open(test_info_file, "r") as file_desc:
            lines = file_desc.readlines()
            for line in lines:
                class_name, _ = line.split(',', 1)
                class_name = class_name.strip()
                if not class_name.endswith("Test"):
                    continue
                java_test_file += class_name + ","
    except(IOError, ValueError) as err_msg:
        print("Error to read info file: ", err_msg)
    if java_test_file != "":
        java_test_file = java_test_file[:-1]
    return java_test_file


def get_dex_test_para(filename, suite_file):
    exec_info = get_java_test_para("", "")
    java_test_file = get_execute_java_test_files(suite_file)
    exec_class, exec_method, exec_level = exec_info
    dex_test_para = "%s  %s%s  %s%s  %s%s  %s%s  %s%s  %s%s" % (
        ZunitConst.z_unit_app, ZunitConst.output_dir,
        DEVICE_TEST_PATH,
        ZunitConst.output_file, filename,
        ZunitConst.test_class, java_test_file,
        ZunitConst.exec_class, exec_class,
        ZunitConst.exec_method, exec_method,
        ZunitConst.exec_level, exec_level)
    return dex_test_para


def make_long_command_file(command, longcommand_path, filename):
    sh_file_name = '%s.sh' % filename
    file_path = os.path.join(longcommand_path, sh_file_name)
    try:
        with open(file_path, "a") as file_desc:
            file_desc.write(command)
    except(IOError, ValueError) as err_msg:
        print("Error for make long command file: ", err_msg)
    return sh_file_name, file_path


##################################################################
##################################################################

def get_package_and_ability_name(hap_filepath):
    package_name = ""
    ability_name = ""

    if os.path.exists(hap_filepath):
        filename = os.path.basename(hap_filepath)

        # unzip the hap file
        hap_bak_path = os.path.abspath(os.path.join(
            os.path.dirname(hap_filepath),
            "%s.bak" % filename))
        zf_desc = zipfile.ZipFile(hap_filepath)
        try:
            zf_desc.extractall(path=hap_bak_path)
        except RuntimeError as error:
            print(error)
        zf_desc.close()

        # verify config.json file
        app_profile_path = os.path.join(hap_bak_path, "config.json")
        if not os.path.exists(app_profile_path):
            print("file %s not exists" % app_profile_path)
            return package_name, ability_name

        if os.path.isdir(app_profile_path):
            print("%s is a folder, and not a file" % app_profile_path)
            return package_name, ability_name

        # get package_name and ability_name value.
        load_dict = {}
        with open(app_profile_path, 'r') as load_f:
            load_dict = json.load(load_f)
        profile_list = load_dict.values()
        for profile in profile_list:
            package_name = profile.get("package")
            if not package_name:
                continue

            abilities = profile.get("abilities")
            for abilitie in abilities:
                abilities_name = abilitie.get("name")
                if abilities_name.startswith("."):
                    ability_name = package_name + abilities_name[
                        abilities_name.find("."):]
                else:
                    ability_name = abilities_name
                break
            break

        # delete hap_bak_path
        if os.path.exists(hap_bak_path):
            shutil.rmtree(hap_bak_path)
    else:
        print("file %s not exists" % hap_filepath)

    return package_name, ability_name


def get_hap_test_para(filename, suite_file):
    if not filename.endswith(INSTRUMENT_HAP_FILE_SUFFIX):
        exec_class, exec_method, exec_level = get_java_test_para("", "")
        java_test_file = get_execute_java_test_files(suite_file)
        junit_test_para = "%s%s#%s%s#%s%s#%s%s" % (
            ZunitConst.test_class, java_test_file,
            ZunitConst.exec_class, exec_class,
            ZunitConst.exec_method, exec_method,
            ZunitConst.exec_level, exec_level)
    else:
        junit_test_para = get_execute_java_test_files(suite_file)
    return junit_test_para


##############################################################################
##############################################################################


class ITestDriver:
    __metaclass__ = ABCMeta

    @abstractmethod
    def execute(self, suite_file, push_flag=False):
        pass


##############################################################################
##############################################################################


class CppTestDriver(ITestDriver):
    def __init__(self, device):
        self.device = device

    def execute(self, suite_file, background=False):
        file_name = os.path.basename(suite_file)

        long_command_path = tempfile.mkdtemp(prefix="long_command_",
            dir=os.path.join(os.environ.get('PYTEST_RESULTPATH'), "temp"))
        command = "cd %s; rm -rf %s.xml; chmod +x *; ./%s" % (
            DEVICE_TEST_PATH,
            file_name,
            file_name)

        print("command: %s" % command)
        sh_file_name, file_path = make_long_command_file(command,
            long_command_path,
            file_name)
        self.device.push_file(file_path, DEVICE_TEST_PATH)

        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.device)
        if background:
            sh_command = "nohup sh %s >%s 2>&1 &" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name),
                os.path.join(DEVICE_TEST_PATH, "agent.log"))
        else:
            sh_command = "sh %s" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name))

        return self.device.shell(sh_command)


class DexTestDriver(ITestDriver):
    def __init__(self, device):
        self.device = device

    def execute(self, suite_file, background=False):
        filename = os.path.basename(suite_file)
        target_test_path = DEVICE_TEST_PATH
        junit_test_para = get_dex_test_para(filename, suite_file)

        long_command_path = tempfile.mkdtemp(prefix="long_command_",
            dir=os.path.join(os.environ.get('PYTEST_RESULTPATH'), "temp"))
        command = "cd %s; rm -rf %s.xml; chmod +x *;" \
                  " export BOOTCLASSPATH=%s%s:$BOOTCLASSPATH;" \
                  " app_process %s%s %s" % \
                  (target_test_path,
                   filename,
                   target_test_path,
                   filename,
                   target_test_path,
                   filename,
                   junit_test_para)

        print("command: %s" % command)
        sh_file_name, file_path = make_long_command_file(command,
            long_command_path,
            filename)
        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.device)
        self.device.push_file(file_path, DEVICE_TEST_PATH)

        if background:
            sh_command = "nohup sh %s 2>&1 &" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name))
        else:
            sh_command = "sh %s" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name))

        return self.device.shell(sh_command)


class HapTestDriver(ITestDriver):
    def __init__(self, device):
        self.device = device
        self.ability_name = ""
        self.package_name = ""
        self.activity_name = ""
        self.target_result_dir = ""

    def execute(self, suite_file, background=False):
        self.package_name, self.ability_name = get_package_and_ability_name(
            suite_file)
        self.activity_name = "%s.MainAbilityShellActivity" % self.package_name
        self.target_result_dir = "/data/data/%s/files/test/result/" % (
            self.package_name)

        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.device)
        filename = os.path.basename(suite_file)
        junit_test_para = get_hap_test_para(filename, suite_file)

        install_result = self.install_hap(filename)
        if install_result:
            self._execute_suitefile_junittest(filename,
                junit_test_para,
                self.target_result_dir,
                background)
            self.move_result_file(filename)
            self.unistall_hap(self.package_name)
        else:
            print("Error: install hap failed.")

    def _execute_suitefile_junittest(self, filename,
                                     testpara,
                                     target_test_path,
                                     background):
        self.device.unlock_screen()
        self.device.unlock_device()

        try:
            if not filename.endswith(INSTRUMENT_HAP_FILE_SUFFIX):
                command = "am start -S -n %s/%s --es param '%s'" % (
                    self.package_name,
                    self.activity_name,
                    testpara)
            else:
                command = "aa start -p %s -n %s -s AbilityTestCase %s" \
                    " -w %s" % (
                    self.package_name,
                    self.ability_name,
                    testpara,
                    str(TIME_OUT))

            print("command: %s" % command)
            long_command_path = tempfile.mkdtemp(prefix="long_command_",
                dir=os.path.join(os.environ.get('PYTEST_RESULTPATH'), "temp"))
            sh_file_name, file_path = make_long_command_file(command,
                long_command_path,
                filename)
            self.device.push_file(file_path, DEVICE_TEST_PATH)

            if background:
                sh_command = "nohup sh %s 2>&1 &" % (
                    os.path.join(DEVICE_TEST_PATH, sh_file_name))
            else:
                sh_command = "sh %s" % (
                    os.path.join(DEVICE_TEST_PATH, sh_file_name))

            return_message = self.device.shell(sh_command)
            time.sleep(1)
            if not filename.endswith(INSTRUMENT_HAP_FILE_SUFFIX):
                print("HAP Testcase is executing, please wait a moment...")
                if return_message:
                    self._check_hap_finished(target_test_path)
        except (ExecuteTerminate, DeviceError) as exception:
            return_message = str(exception.args)
            print("return_message")

        self.device.lock_screen()

    def _check_hap_finished(self, target_test_path):
        run_timeout = True
        sleep_duration = 3
        target_file = os.path.join(target_test_path,
            ZunitConst.jtest_status_filename)

        for _ in range(int(TIME_OUT / (1000 * sleep_duration))):
            check_value = self.device.is_file_exist(target_file)
            if not check_value:
                time.sleep(sleep_duration)
                continue
            run_timeout = False
            break

        if run_timeout:
            return_code = False
            print("HAP Testcase executed timeout or exception.")
        else:
            return_code = True
            print("HAP Testcase executed finished")

        return return_code

    def move_result_file(self, filename):
        remote_result_file = os.path.join(self.target_result_dir,
            "testcase_result.xml")
        self.device.shell("mv %s %s.xml" % (remote_result_file,
            os.path.join(DEVICE_TEST_PATH, filename)))

    def install_hap(self, filename):
        message = self.device.shell_with_output("bm install -p %s" %
            os.path.join(DEVICE_TEST_PATH, filename))
        message = str(message).rstrip()
        if message != "":
            print(message)

        if message == "" or "Success" in message:
            return_code = True
        else:
            return_code = False

        time.sleep(1)
        return return_code

    def unistall_hap(self, package_name):
        result = self.device.shell("pm uninstall %s" % package_name)
        time.sleep(1)
        return result


##############################################################################
##############################################################################

