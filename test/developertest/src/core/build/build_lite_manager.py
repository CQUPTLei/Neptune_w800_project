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

import platform
import subprocess

from xdevice import platform_logger


class BuildLiteManager(object):
    """
    build lite system version or test cases
    build param:
    device type:for examples, watch or camera
    board platform:for examples, HI3518ev300 or HI3516DV300
    kernel type:for examples, liteos_m, liteos_a, linux
    """
    log = platform_logger("BuildLiteManager")

    def __init__(self):
        self.board_series = ""
        self.board_type = ""
        self.board_product = ""

    def build_version_and_cases(self):
        build_command = "hb build -b debug"
        self.log.info("build param:%s" % build_command)
        build_result = False
        try:
            build_result = subprocess.call(build_command) == 0
        except IOError as exception:
            self.log.error("build test case failed, exception=%s" % exception)
        return build_result

    def exec_build_test(self, param_option):
        """
        build os lite version and test cases
        :param param_option: build param
        :return:build success or failed
        """
        if platform.system() == "Linux":
            return self.build_version_and_cases()
        self.log.info("windows environment, only use .bin test cases")
        return True

