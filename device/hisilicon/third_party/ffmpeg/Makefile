LOCAL_DIR := $(shell pwd)
MIDDLEWARE_DIR := $(LOCAL_DIR)
include $(MIDDLEWARE_DIR)/cfg.mak

GREEN="\e[32;1m"
DONE="\e[39m"
RED="\e[31m"

COMPILE_ROOT := $(LOCAL_DIR)
FFMPEG_VER := ffmpeg-y
ifeq ($(CFG_CHIP_TYPE), hi3516cv300)
FF_CONFIG_SH := enable_decoder_config.sh
#else ifeq ($(CFG_CHIP_TYPE), hi3518ev300)
#FF_CONFIG_SH := enable_decoder_config.sh
else
FF_CONFIG_SH := disable_decoder_config.sh
endif

export LITEOS_MACRO
export LITEOS_OSDRV_INCLUDE
export LITEOS_USR_INCLUDE
export LITEOS_CMACRO
export LITEOS_INCLUDE
export LITEOSTOPDIR

#override CFLAGS to avoid ffmpeg configure use
CFLAGS:=
FF_ADAPT_LITEOS=y
#export LLVM_COMPILER=y
ifeq ($(CFG_OS_TYPE), linux)
export LLVM_COMPILER=n
CONFIGURE_FILE := configure_gcc
else
export LLVM_COMPILER=y
CONFIGURE_FILE := configure_llvm
endif

ORG_FFMPEG := $(FFMPEG_VER)

MODNAME := $(FFMPEG_VER)

all: $(ORG_FFMPEG)

.PHONY: $(ORG_FFMPEG) clean

install:$(ORG_FFMPEG)

clean:
	@if  [ -d $(ORG_FFMPEG)/install ]; then \
	cd $(COMPILE_ROOT)/$(ORG_FFMPEG);$(MAKE) $(MFLAGS) distclean;$(MAKE) $(MFLAGS) uninstall;rm -rf $(COMPILE_ROOT)/$(ORG_FFMPEG)/install; \
	fi

$(ORG_FFMPEG):
	cp $(CONFIGURE_FILE) $(FFMPEG_VER)/configure
	@if  [ -d $(FFMPEG_VER) ]; then cd $@; chmod 777 ./* -R; ./$(FF_CONFIG_SH) $(CFG_CHIP_TYPE) $(CFG_OS_TYPE) $(CFG_LINUX_COMPILER_VER) $(CFG_OHOS_BUILD_PATH); cd -; fi
	@if  [ $(FF_ADAPT_LITEOS) = 'y' ]; then cd $@; ./adapt_liteos_config.sh; cd -; fi
	$(MAKE) $(MFLAGS) -j16 -C $(COMPILE_ROOT)/$@
	$(MAKE) $(MFLAGS) -C $(@) install
