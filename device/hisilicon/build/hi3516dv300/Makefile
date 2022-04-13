
LITEOSTOPDIR := $(shell pwd)/../../../../kernel/liteos_a
CONFIG_FILE := $(LITEOSTOPDIR)/.config

ifeq ($(wildcard $(CONFIG_FILE)), )
$(error  "########################kernel is not compiled################################")
endif
include $(LITEOSTOPDIR)/config.mk

ifeq ($(LOSCFG_COMPILER_CLANG_LLVM), y)
MPP_DIR := $(LITEOSTOPDIR)/../../device/hisilicon/hispark_taurus/sdk_liteos/mpp/module_init/
else ifeq ($(LOSCFG_COMPILER_HIMIX_32), y)
MPP_DIR := $(LITEOSTOPDIR)/../../device/hisilicon/hi3516dv300/sdk_liteos/mpp/
else
$(error  "########################compiler is not supported################################")
endif

include hi3516dv300.mak
