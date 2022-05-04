TOP_DIR := .
sinclude $(TOP_DIR)/tools/w800/conf.mk

ifndef PDIR # {
GEN_IMAGES= $(TARGET).elf
GEN_BINS = $(TARGET).bin
SUBDIRS =
ifeq ($(USE_DEMO_APP), true)
SUBDIRS += \
    $(TOP_DIR)/app \
    $(TOP_DIR)/demo
endif # USE_DEMO_APP
endif # } PDIR

ifndef PDIR # {
ifeq ($(USE_LIB), 0)
SUBDIRS += \
    $(TOP_DIR)/platform/arch        \
    $(TOP_DIR)/platform/common      \
    $(TOP_DIR)/platform/drivers     \
    $(TOP_DIR)/platform/sys         \
    $(TOP_DIR)/src/network          \
    $(TOP_DIR)/src/os               \
    $(TOP_DIR)/src/app
endif
endif

COMPONENTS_$(TARGET) :=


#ifeq ($(USE_DEMO_APP), true)

#COMPONENTS_$(TARGET) += \
#    $(TOP_DIR)/app/libuser$(LIB_EXT) \
#    $(TOP_DIR)/demo/libdemo$(LIB_EXT) \

#endif

#ifeq ($(USE_LIB), 0)
#COMPONENTS_$(TARGET) += \
#    $(TOP_DIR)/platform/boot/libwmarch$(LIB_EXT)        \
#    $(TOP_DIR)/platform/common/libwmcommon$(LIB_EXT)    \
#    $(TOP_DIR)/platform/drivers/libdrivers$(LIB_EXT)    \
#    $(TOP_DIR)/platform/sys/libwmsys$(LIB_EXT)          \
#    $(TOP_DIR)/src/network/libnetwork$(LIB_EXT)         \
#    $(TOP_DIR)/src/os/libos$(LIB_EXT)                   \
#    $(TOP_DIR)/src/app/libapp$(LIB_EXT)
#endif


LINKLIB = 	\
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwlan$(LIB_EXT) \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libbt$(LIB_EXT)	\
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libdsp$(LIB_EXT)	\


#add by ohos
OHOS_ROOT = $(TOP_DIR)/../../../..
OHOS_LIBS_DIR = $(OHOS_ROOT)/out/neptune/wifiiot_neptune/libs

LINKLIB += 	\
	$(OHOS_LIBS_DIR)/libhal_file_static.a          \
	$(OHOS_LIBS_DIR)/libhievent_lite.a             \
	$(OHOS_LIBS_DIR)/libcmsis.a                    \
	$(OHOS_LIBS_DIR)/libhilog_lite.a               \
	$(OHOS_LIBS_DIR)/libwifiservice.a              \
	$(OHOS_LIBS_DIR)/libtoken_static.a             \
	$(OHOS_LIBS_DIR)/libhal_token_static.a         \
	$(OHOS_LIBS_DIR)/libhichainsdk.a               \
	$(OHOS_LIBS_DIR)/libhal_sysparam.a             \
	$(OHOS_LIBS_DIR)/libhiview_lite.a              \
	$(OHOS_LIBS_DIR)/libsamgr_adapter.a            \
	$(OHOS_LIBS_DIR)/libbootstrap.a                \
	$(OHOS_LIBS_DIR)/libdump_static.a              \
	$(OHOS_LIBS_DIR)/libhilog_lite_command.a       \
	$(OHOS_LIBS_DIR)/libhal_iothardware.a          \
	$(OHOS_LIBS_DIR)/libbroadcast.a                \
	$(OHOS_LIBS_DIR)/libsamgr_source.a             \
	$(OHOS_LIBS_DIR)/libarch.a                     \
	$(OHOS_LIBS_DIR)/libutils.a                    \
	$(OHOS_LIBS_DIR)/libsysparam.a                 \
	$(OHOS_LIBS_DIR)/libsamgr.a                    \
	$(OHOS_LIBS_DIR)/libnative_file.a              \
	$(OHOS_LIBS_DIR)/libkernel.a                   \
	$(OHOS_LIBS_DIR)/libutils_kv_store.a           \
	$(OHOS_LIBS_DIR)/libsec_static.a               \
	$(OHOS_LIBS_DIR)/libbacktrace.a                \
	$(OHOS_LIBS_DIR)/libhuks.a                     \
	$(OHOS_LIBS_DIR)/libmaster.a                   \

# append harmony os libraries
LINKLIB += $(sort $(wildcard $(OHOS_LIB)/*.a))
# $(info LINKLIB is $(LINKLIB))

ifeq ($(USE_LIB), 1)
LINKLIB += \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libapp$(LIB_EXT)         \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwmarch$(LIB_EXT)      \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwmcommon$(LIB_EXT)    \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libdrivers$(LIB_EXT)     \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libnetwork$(LIB_EXT)     \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libos$(LIB_EXT)          \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwmsys$(LIB_EXT)
endif

LINKFLAGS_$(TARGET) =  \
	$(LINKLIB)

CONFIGURATION_DEFINES =	

DEFINES +=				\
	$(CONFIGURATION_DEFINES)

DDEFINES +=				\
	$(CONFIGURATION_DEFINES)

INCLUDES := $(INCLUDES) -I$(PDIR)include
INCLUDES += -I ./

sinclude $(TOP_DIR)/tools/$(CONFIG_ARCH_TYPE)/rules.mk

.PHONY: FORCE
FORCE:
