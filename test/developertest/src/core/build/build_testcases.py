#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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
import json
import shutil
import subprocess
import platform
from xdevice import platform_logger
from core.utils import get_build_output_path
from core.config.config_manager import UserConfigManager

BUILD_FILEPATH = "build/gn/build_ohos.sh"
BUILD_LITE_FILE = "build/lite/build.py"
LOG = platform_logger("BuildTestcases")


##############################################################################
##############################################################################

class BuildTestcases(object):
    def __init__(self, project_rootpath):
        self.project_rootpath = project_rootpath

        user_manager = UserConfigManager()
        self.is_doublefwk = user_manager.get_user_config_flag(
            "common", "doublefwk")
        self.is_build_example = user_manager.get_user_config_flag(
            "build", "example")
        self.build_paramter_dic = user_manager.get_user_config(
            "build", "paramter")

    @classmethod
    def _copy_folder(cls, source_dir, target_dir):
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)

        if platform.system() == 'Windows':
            command = ["xcopy", "/f", "/s", "/e", "/y",
                       "/b", "/q", source_dir, target_dir]
        else:
            command = ["cp", "-rf", source_dir, target_dir]

        LOG.info("command: %s" % str(command))
        return subprocess.call(command) == 0

    @classmethod
    def _get_testcase_outname_by_productform(cls, productform):
        if productform == "phone":
            return ""

        testcase_outpath = ""
        toolchain_filepath = os.path.join(
            get_build_output_path(),
            "build_configs",
            "platforms_info",
            "toolchain_to_variant.json")
        if os.path.exists(toolchain_filepath):
            data_dic = []
            with open(toolchain_filepath, 'r') as toolchain_file:
                data_dic = json.load(toolchain_file)
                if not data_dic:
                    LOG.warning("The %s file load error." %
                                toolchain_filepath)
                    data_dic = []
            paltform_toolchain_dic = data_dic.get("paltform_toolchain")
            testcase_outpath = paltform_toolchain_dic.get(productform, "")
            pos = testcase_outpath.rfind(':') + 1
            testcase_outpath = testcase_outpath[pos:len(testcase_outpath)]
        return testcase_outpath

    def _delete_testcase_dir(self, productform):
        package_out_dir = os.path.join(
            get_build_output_path(),
            "packages",
            productform, "tests")
        LOG.info("package_out_dir=%s" % package_out_dir)
        if os.path.exists(package_out_dir):
            shutil.rmtree(package_out_dir)

        phone_out_dir = os.path.join(
            self.project_rootpath, "out", "release", "tests")
        LOG.info("phone_out_dir=%s" % phone_out_dir)
        if os.path.exists(phone_out_dir):
            shutil.rmtree(phone_out_dir)

        curr_productform_outname = self._get_testcase_outname_by_productform(
            productform)
        if curr_productform_outname == "":
            return

        curr_productform_outdir = os.path.join(
            get_build_output_path(),
            curr_productform_outname,
            "tests")
        LOG.info("curr_productform_outdir=%s" % curr_productform_outdir)
        if os.path.exists(curr_productform_outdir):
            shutil.rmtree(curr_productform_outdir)

    def _merge_testcase_dir(self, productform):
        package_out_dir = os.path.join(
            get_build_output_path(),
            "packages",
            productform)
        if platform.system() == 'Windows':
            package_out_dir = os.path.join(package_out_dir, "tests")

        phone_out_dir = os.path.join(get_build_output_path(), "tests")
        if os.path.exists(phone_out_dir):
            self._copy_folder(phone_out_dir, package_out_dir)

        curr_productform_outname = self._get_testcase_outname_by_productform(
            productform)
        if curr_productform_outname == "":
            return

        curr_productform_outdir = os.path.join(
            get_build_output_path(),
            curr_productform_outname,
            "tests")
        LOG.info("curr_productform_outdir=%s" % curr_productform_outdir)
        if os.path.exists(curr_productform_outdir):
            self._copy_folder(curr_productform_outdir, package_out_dir)

    def _execute_build_command(self, command):
        build_result = False
        current_path = os.getcwd()
        os.chdir(self.project_rootpath)

        if os.path.exists(BUILD_FILEPATH):
            build_command = [BUILD_FILEPATH]
            build_command.extend(command)
            LOG.info("build_command: %s" % str(build_command))
            if subprocess.call(build_command) == 0:
                build_result = True
            else:
                build_result = False
        else:
            if not self.is_doublefwk:
                build_result = self._execute_build_lite_command()
            else:
                LOG.warning("Error: The %s is not exist" % BUILD_FILEPATH)

        os.chdir(current_path)
        return build_result

    def _execute_build_lite_command(self):
        if os.path.exists(BUILD_LITE_FILE):
            temp_user_manager = UserConfigManager()
            build_command_config = \
                temp_user_manager.get_user_config("build", "board_info")
            build_command = [build_command_config.get("build_command", "")]
            LOG.info("build_command: %s" % str(build_command))
            try:
                if subprocess.call(build_command) == 0:
                    LOG.info("execute build lite command success")
                    return True
                else:
                    LOG.error("execute build lite command failed")
            except IOError as exception:
                LOG.error("build lite test case failed, exception=%s"
                          % exception)
        else:
            LOG.warning("Error: The %s is not exist" % BUILD_LITE_FILE)
        return True

    def build_testcases(self, productform, target):
        command = []

        target_os = self.build_paramter_dic.get("target_os")
        command.append("target_os=" + target_os)

        target_cpu = self.build_paramter_dic.get("target_cpu")
        command.append("target_cpu=" + target_cpu)

        if self.is_doublefwk:
            command.append("double_framework=true")

        if self.is_build_example:
            command.append("build_example=true")

        command.append("target_platform=" + productform.lower())
        command.append("build_target=" + target)

        self._delete_testcase_dir(productform)
        build_result = self._execute_build_command(command)
        self._merge_testcase_dir(productform)
        return build_result

    def build_gn_file(self, productform):
        command = []

        target_os = self.build_paramter_dic.get("target_os")
        command.append("target_os=" + target_os)

        target_cpu = self.build_paramter_dic.get("target_cpu")
        command.append("target_cpu=" + target_cpu)

        if self.is_doublefwk:
            command.append("double_framework=true")

        if self.is_build_example:
            command.append("build_example=true")

        command.append("target_platform=" + productform.lower())
        command.append("build_only_gn=true")

        return self._execute_build_command(command)

    def build_version(self, productform):
        command = []

        target_os = self.build_paramter_dic.get("target_os")
        command.append("target_os=" + target_os)

        target_cpu = self.build_paramter_dic.get("target_cpu")
        command.append("target_cpu=" + target_cpu)

        if self.is_doublefwk:
            command.append("double_framework=true")

        command.append("target_platform=" + productform.lower())
        command.append("build_target=make_all")

        return self._execute_build_command(command)

    def build_all_testcases(self):
        command = []
        if self.is_doublefwk:
            command.append("double_framework=true")
        if self.is_build_example:
            command.append("build_example=true")
        command.append("build_target=make_test")
        command.append("target_platform=all")
        return self._execute_build_command(command)


##############################################################################
##############################################################################
