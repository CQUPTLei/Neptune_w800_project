#!/usr/bin/env python
#-*-coding:utf-8-*-

#Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

# Perform for Z kernel headers

import sys
import os
import getopt
import subprocess
import time
import string
import re
import glob

# /*..*/  //...
Rule1 = "(\/\*(\s|.)*?\*\/)|(\/\/.*)"
c1=re.compile(Rule1)

#use list for order
key_list = [
#     grep key,								gerp dir,				sed key,								sed replace
    ["CONFIG_64BIT",						".",					"CONFIG_64BIT",							"__LP64__"],
    ["__unused;",							".",					"__unused;",							"__linux_unused;"],
    ["__unused\[",							".",					"__unused\[",							"__linux_unused["],
    [" private;",							".",					" private;",							" __linux_private;"],
    ["\*virtual;",							".",					"\*virtual;",							"\* __linux_virtual;"],
    ["struct msqid_ds",						".",					"struct msqid_ds",						"struct __kernel_legacy_msqid_ds"],
    ["struct semid_ds",						".",					"struct semid_ds",						"struct __kernel_legacy_semid_ds"],
    ["struct shmid_ds",						".",					"struct shmid_ds",						"struct __kernel_legacy_shmid_ds"],
    ["struct ipc_perm",						".",					"struct ipc_perm",						"struct __kernel_legacy_ipc_perm"],
    ["union semun",							".",					"union semun",							"union __kernel_legacy_semun"],
    ["_NSIG",								".",					"_NSIG",								"_KERNEL__NSIG"],
    ["_KERNEL__NSIG_",						".",					"_KERNEL__NSIG_",						"_NSIG_"],
    ["#define NSIG\s",						".",					"#define NSIG",							"#define _KERNEL_NSIG"],
    ["SIGRTMIN",							".",					"SIGRTMIN",								"__SIGRTMIN"],
    ["SIGRTMAX",							".",					"SIGRTMAX",								"__SIGRTMAX"],
    ["struct udphdr",						".",					"struct udphdr",						"struct __kernel_udphdr"],
    ["__attribute_const__",					".",					"__attribute_const__",					"__attribute__((__const__))"],
    ["struct __kernel_sockaddr_storage",	".",					"struct __kernel_sockaddr_storage",		"struct sockaddr_storage"],
    ["off_t",								"asm-mips/asm/stat.h",	"off_t",								"__kernel_off_t"],
    ["#ifndef __EXPORTED_HEADERS__",		".",					"#ifndef __EXPORTED_HEADERS__",			"#if 0 \/*__EXPORTED_HEADERS__*\/"],
    ["#ifndef __KERNEL_STRICT_NAMES",		".",					"#ifndef __KERNEL_STRICT_NAMES",		"#if 0 \/*__KERNEL_STRICT_NAMES*\/"],
    ["#ifdef __CHECKER__",					".",					"#ifdef __CHECKER__",					"#if 0 \/*__CHECKER__*\/"],
    ["#ifdef __HAVE_BUILTIN_BSWAP16__",		".",					"#ifdef __HAVE_BUILTIN_BSWAP16__",		"#if 1 \/*__HAVE_BUILTIN_BSWAP16__*\/"],
    ["#ifdef __HAVE_BUILTIN_BSWAP32__",		".",					"#ifdef __HAVE_BUILTIN_BSWAP32__",		"#if 1 \/*__HAVE_BUILTIN_BSWAP32__*\/"],
    ["#ifdef __HAVE_BUILTIN_BSWAP64__",		".",					"#ifdef __HAVE_BUILTIN_BSWAP64__",		"#if 1 \/*__HAVE_BUILTIN_BSWAP64__*\/"],
    ["#if defined(__ARMEB__)",				"asm-arm",				"#if defined(__ARMEB__)",				"#if 0 \/*__ARMEB__*\/"],
    ["#ifdef __ARMEB__",					"asm-arm",				"#ifdef __ARMEB__",						"#if 0 \/*__ARMEB__*\/"],
    ["defined(__ARM_EABI__)",				"asm-arm",				"defined(__ARM_EABI__)",				"1 \/*defined(__ARM_EABI__)*\/"],
    ["#ifdef __MIPSEB__",					"asm-mips",				"#ifdef __MIPSEB__",					"#if 0 \/*__MIPSEB__*\/"],
    ["#elif defined (__MIPSEB__)",			"asm-mips",				"#elif defined (__MIPSEB__)",			"#elif 0 \/*__MIPSEB__*\/"],
    ["#if defined(__MIPSEB__)",				"asm-mips",				"#if defined(__MIPSEB__)",				"#if 0 \/*__MIPSEB__*\/"],
    ["#elif defined(__MIPSEL__)",			"asm-mips",				"#elif defined(__MIPSEL__)",			"#elif 1 \/*__MIPSEL__*\/"],
    ["#elif defined (__MIPSEL__)",			"asm-mips",				"#elif defined (__MIPSEL__)",			"#elif 1 \/*__MIPSEL__*\/"],
    ["\!defined(__SANE_USERSPACE_TYPES__)",	"asm-mips",				"\!defined(__SANE_USERSPACE_TYPES__)",	"0 \/*__SANE_USERSPACE_TYPES__*\/"],
    ["\/\*.*\*\/",						".",					"\/\*.*\*\/",							""],
    ]

def deal_file(src):
    # file exist or not
    if not os.path.exists(src):
        print 'Error: file - %s doesn\'t exist.'% src
        return False
    if os.path.islink(src):
        print 'Error: file - %s is a link.'
        return False
    filetype = (os.path.splitext(src))[1]
    if not filetype in ['.c','.h','.cpp','.hh','.cc']:
        return False
    try:
        if not os.access(src, os.W_OK):
            os.chmod(src, 0664)
    except:
        print 'Error: you can not chang %s\'s mode.'% src

    # del all comments
    inputf = open(src, 'r')
    outputfilename = (os.path.splitext(src))[0] + '_no_comment'+filetype
    outputf = open(outputfilename, 'w')

    lines=inputf.read()
    inputf.close()
    lines=re.sub(Rule1,"",lines)
    outputf.write(lines)
    outputf.close()

    #del right space & blank line
    os.remove(src)
    inputf  = open(outputfilename, 'r')
    outputf = open(src, 'w')
    #write head info
    outputf.write("/*\n")
    outputf.write(" * This header was generated from the Linux kernel headers by update_headers.py,\n")
    outputf.write(" * to provide necessary information from kernel to userspace, such as constants,\n")
    outputf.write(" * structures, and macros, and thus, contains no copyrightable information.\n")
    outputf.write(" */\n")
    line = inputf.readline()
    while line:
        if line == '\n':
            line = line.strip("\n")
        else:
            line = line.rstrip() + '\n'
        outputf.write(line)
        line = inputf.readline()
    inputf.close()
    outputf.close()
    os.remove(outputfilename)

    return True


def deal_dir(src):
    #  dir exist or not
    if not os.path.exists(src):
        print 'Error: dir - %s is not exist.'%s (src)
        return False
    filelists = os.listdir(src)
    for eachfile in filelists:
        eachfile = src + '/' +eachfile
        if os.path.isdir(eachfile):
            deal_dir(eachfile)
        elif os.path.isfile(eachfile):
            deal_file(eachfile)
    return True


def copy_kernel_headers(build_top, original_dir, target_dir):
    headers_dir = build_top + '/' + target_dir
    external_dir = build_top + original_dir

    headers_rm_cmd = "rm " + headers_dir + "/uapi -rf"
    headers_cp_cmd = "cp " + external_dir + " " + headers_dir + " -r"

    try:
        status = subprocess.call(headers_rm_cmd, shell=True)
    except:
        printf("[Error] rm error!!!")

    try:
        status = subprocess.call(headers_cp_cmd, shell=True)
    except:
        printf("[Error] cp error!!!")

    return 1

def replace_key_words_ext(keys):
    replace_key_words(keys[0], keys[1], keys[2], keys[3])
    return 1

def replace_key_words(original_key, target_dir, replace_key, replace_str):
    replace_cmd = "grep \""+ original_key + "\" -rl " + target_dir + " | xargs sed -i \"s/" + replace_key + "/" + replace_str + "/g\""
    try:
        status = subprocess.call(replace_cmd, shell=True)
    except:
        printf("[Error] replace CONFIG_64BIT error!!!")

    return 1



def usage():
    print("    Usage: run the script at the root of project.\n    For example:\n\
      python ./foundation/ccruntime/libc/kernel/tools/update_Z_header.py")
    return 0


def main():

    if len(sys.argv) > 1:
        usage()
        return -1

    kernel_dir = "prebuilts/lite/sysroot/ohos_tmp"
    external_dir = '/prebuilts/lite/sysroot/ohos_tmp_ori/uapi'
    status = -1

    try:
        build_top = os.environ["Z_BUILD_TOP"]
    except KeyError as e:
        print("[Warning] no env : %s, use current work dir." %(str(e)))
        build_top = subprocess.check_output("pwd", shell=True)

    status = copy_kernel_headers(build_top.strip(), external_dir, kernel_dir)
    if status != 1:
        print("[Error] copy files fail!!!")
        sys.exti(1)

    try:
        os.chdir(kernel_dir + "/uapi")
    except:
        print("[Error] no uapi dir1!!!")
        sys.exit(1)

    current_dir = os.getcwd()

    if "uapi" not in current_dir:
        print("[Error] not uapi dir!!!")
        sys.exit(1)

    for val in key_list:
        replace_key_words_ext(val)

    try:
        os.chdir("../")
    except:
        print("[Error] no uapi dir2!!!")
        sys.exit(1)
    deal_dir("uapi")

    return 1

if __name__ == "__main__":
    starttime=time.time()
    main()
    endtime=time.time()
    print(endtime-starttime)

