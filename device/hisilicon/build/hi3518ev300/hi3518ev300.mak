# Copyright (c) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
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

# Hisilicon Hi35xx sample Makefile.param
REL_LIB := $(MPP_DIR)/lib
REL_INC := $(MPP_DIR)/include
ARCH_LIBNAME := hi3516ev200
################# select sensor type for your sample #################
####    SONY_IMX335_MIPI_5M_30FPS_12BIT              #################
####    SONY_IMX335_MIPI_4M_30FPS_12BIT              #################
####    SONY_IMX327_MIPI_2M_30FPS_12BIT              #################
####    SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1      #################
####    SONY_IMX307_MIPI_2M_30FPS_12BIT              #################
####    SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1      #################
####    SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1      #################
####    OMNIVISION_OS05A_MIPI_5M_30FPS_12BIT         #################
####    OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT       #################
####    BT1120_2M_30FPS_8BIT                         #################

#### the previous sequence only for Hi3516EV300,Hi3516DV200 etc. #####

####    BT656_2M_30FPS_8BIT                          #################
####    SONY_IMX307_2L_MIPI_2M_30FPS_12BIT           #################
####    SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1   #################
####    SONY_IMX327_2L_MIPI_2M_30FPS_12BIT           #################
####    SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1   #################
####    SMART_SC4236_MIPI_3M_30FPS_10BIT             #################
####    SMART_SC4236_MIPI_3M_20FPS_10BIT             #################
####    SMART_SC2235_DC_2M_30FPS_10BIT               #################
####    SMART_SC2231_MIPI_2M_30FPS_10BIT             #################
####    SOI_JXF37_MIPI_2M_30FPS_10BIT                #################
####    SMART_SC3235_MIPI_3M_30FPS_10BIT             #################
####    GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT        #################
####    GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT_FORCAR #################
####    SUPERPIX_SP2305_MIPI_2M_30FPS_10BIT          #################

SENSOR0_TYPE ?= SONY_IMX307_2L_MIPI_2M_30FPS_12BIT

#SENSOR_LIBS := $(REL_LIB)/lib_hiae.a
#SENSOR_LIBS += $(REL_LIB)/libisp.a
#SENSOR_LIBS += $(REL_LIB)/lib_hidehaze.a
##SENSOR_LIBS += $(REL_LIB)/lib_hidrc.a
#SENSOR_LIBS += $(REL_LIB)/lib_hildci.a
#SENSOR_LIBS += $(REL_LIB)/lib_hiawb.a
#SENSOR_LIBS += $(REL_LIB)/lib_hicalcflicker.a
#SENSOR_LIBS += $(REL_LIB)/lib_hiir_auto.a
#SENSOR_LIBS += $(REL_LIB)/lib_hiacs.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx327.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx307.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx307_2l.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx327_2l.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx335.a
SENSOR_LIBS += $(REL_LIB)/libsns_sc4236.a
SENSOR_LIBS += $(REL_LIB)/libsns_sc2235.a
SENSOR_LIBS += $(REL_LIB)/libsns_sc2231.a
SENSOR_LIBS += $(REL_LIB)/libsns_f37.a
SENSOR_LIBS += $(REL_LIB)/libsns_sc3235.a
SENSOR_LIBS += $(REL_LIB)/libsns_gc2053.a
SENSOR_LIBS += $(REL_LIB)/libsns_gc2053_forcar.a
SENSOR_LIBS += $(REL_LIB)/libsns_sp2305.a
SENSOR_LIBS += $(REL_LIB)/libsns_os05a.a
SENSOR_LIBS += $(REL_LIB)/libsns_os04b10.a
#SENSOR_LIBS += $(REL_LIB)/lib_hicalcflicker.a
#MPI_LIBS := $(REL_LIB)/libmpi.a


INC_FLAGS := -I$(REL_INC)

ISP_VERSION := ISP_V2

CFLAGS += -Wall $(INC_FLAGS) -D$(ISP_VERSION)
CFLAGS += -DSENSOR0_TYPE=$(SENSOR0_TYPE)


################ select audio codec type for your sample ################
ACODEC_TYPE ?= ACODEC_TYPE_INNER
#external acodec
#ACODEC_TYPE ?= ACODEC_TYPE_TLV320AIC31
INNER_HDMI ?= n

ifeq ($(ACODEC_TYPE), ACODEC_TYPE_INNER)
	CFLAGS += -DHI_ACODEC_TYPE_INNER
endif

ifeq ($(ACODEC_TYPE), ACODEC_TYPE_TLV320AIC31)
	CFLAGS += -DHI_ACODEC_TYPE_TLV320AIC31
	INC_FLAGS += -I$(SDK_PATH)/$(EXTDRV)/tlv320aic31
endif

ifeq ($(INNER_HDMI), y)
	CFLAGS += -DHI_ACODEC_TYPE_HDMI
endif

################ select audio module library type for your sample ################
AUDIO_MODULE_LIB_TYPE ?= AUDIO_LIB_TYPE_STATIC
#AUDIO_MODULE_LIB_TYPE ?= AUDIO_LIB_TYPE_SHARE
#ifeq ($(AUDIO_MODULE_LIB_TYPE), AUDIO_LIB_TYPE_STATIC)
###### VQE #####
#	CFLAGS += -DHI_VQE_USE_STATIC_MODULE_REGISTER
#	AUDIO_LIBA += $(REL_LIB)/libhive_RES.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_record.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_HPF.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_ANR.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_AEC.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_AGC.a
#	AUDIO_LIBA += $(REL_LIB)/libhive_EQ.a
###### AAC #####
#	CFLAGS += -DHI_AAC_USE_STATIC_MODULE_REGISTER
#	AUDIO_LIBA += $(REL_LIB)/libaacdec.a
#	AUDIO_LIBA += $(REL_LIB)/libaacenc.a
#	AUDIO_LIBA += $(REL_LIB)/libaaccomm.a/AUDIO_LIBA
#	CFLAGS += -DHI_AAC_HAVE_SBR_LIB
#	AUDIO_LIBA += $(REL_LIB)/libaacsbrdec.a
#	AUDIO_LIBA += $(REL_LIB)/libaacsbrenc.a
#endif

#########################################################################

TARGET := OHOS_Image

TARGET_PATH := $(PWD)

# compile OHOS

SDK_LIB_PATH := -L$(REL_LIB) -L$(REL_LIB)/extdrv

#SDK_LIB := $(SDK_LIB_PATH) --start-group -lsys_config -lhi_osal -l$(ARCH_LIBNAME)_base -l$(ARCH_LIBNAME)_sys -lhi_user -l$(ARCH_LIBNAME)_isp -l$(ARCH_LIBNAME)_vi \
#		-l$(ARCH_LIBNAME)_vo -lhifb -l$(ARCH_LIBNAME)_vpss -l$(ARCH_LIBNAME)_vgs -l$(ARCH_LIBNAME)_tde -lhi_mipi_rx\
#		-l$(ARCH_LIBNAME)_chnl -l$(ARCH_LIBNAME)_rc -l$(ARCH_LIBNAME)_rgn -l$(ARCH_LIBNAME)_vedu \
#		-l$(ARCH_LIBNAME)_venc -l$(ARCH_LIBNAME)_h265e -l$(ARCH_LIBNAME)_jpege -l$(ARCH_LIBNAME)_h264e \
#	    -l_hildci -l_hiawb -l_hiae -lisp  -lhi_sensor_i2c -lhi_sensor_spi\
#		-l$(ARCH_LIBNAME)_ai -l$(ARCH_LIBNAME)_ao -l$(ARCH_LIBNAME)_aio -l$(ARCH_LIBNAME)_aenc -l$(ARCH_LIBNAME)_adec -l$(ARCH_LIBNAME)_acodec\
#		-lhi_sensor_spi -lhi_pwm -lhi_piris -lhi_sil9024 -lhi_ssp_ota5182 \
#		-l$(ARCH_LIBNAME)_ive -live -lmd -livp -l$(ARCH_LIBNAME)_cipher\
#		-end-group
SDK_LIB := $(SDK_LIB_PATH) --start-group -lsys_config -lhi_osal -l$(ARCH_LIBNAME)_base -l$(ARCH_LIBNAME)_sys -l$(ARCH_LIBNAME)_isp -l$(ARCH_LIBNAME)_vi \
							-l$(ARCH_LIBNAME)_vo -lhifb -l$(ARCH_LIBNAME)_vpss -l$(ARCH_LIBNAME)_vgs -l$(ARCH_LIBNAME)_tde -lhi_mipi_rx \
							-l$(ARCH_LIBNAME)_chnl -l$(ARCH_LIBNAME)_rc -l$(ARCH_LIBNAME)_rgn -l$(ARCH_LIBNAME)_vedu \
							-l$(ARCH_LIBNAME)_venc -l$(ARCH_LIBNAME)_h265e -l$(ARCH_LIBNAME)_jpege -l$(ARCH_LIBNAME)_h264e \
							-lhi_sensor_i2c -lhi_sensor_spi \
							-l$(ARCH_LIBNAME)_ai -l$(ARCH_LIBNAME)_ao -l$(ARCH_LIBNAME)_aio -l$(ARCH_LIBNAME)_aenc -l$(ARCH_LIBNAME)_adec -l$(ARCH_LIBNAME)_acodec\
							-lhi_sensor_spi -lhi_pwm -lhi_piris \
							-l$(ARCH_LIBNAME)_ive -l$(ARCH_LIBNAME)_cipher\
							--end-group

#SDK_LIB := $(SDK_LIB_PATH) --start-group -lsys_config -lhi_osal -l$(ARCH_LIBNAME)_base -l$(ARCH_LIBNAME)_sys -lhi_user -l$(ARCH_LIBNAME)_isp -l$(ARCH_LIBNAME)_vi \
#							-l$(ARCH_LIBNAME)_vo -lhifb -l$(ARCH_LIBNAME)_vpss -l$(ARCH_LIBNAME)_vgs -l$(ARCH_LIBNAME)_tde -lhi_mipi_rx -lhi_mipi_tx\
#							-l$(ARCH_LIBNAME)_chnl -l$(ARCH_LIBNAME)_rc -l$(ARCH_LIBNAME)_rgn -l$(ARCH_LIBNAME)_vedu \
#							-l$(ARCH_LIBNAME)_venc -l$(ARCH_LIBNAME)_h265e -l$(ARCH_LIBNAME)_jpege -l$(ARCH_LIBNAME)_jpegd -l$(ARCH_LIBNAME)_h264e \
#							-l$(ARCH_LIBNAME)_vdec -l$(ARCH_LIBNAME)_hdmi -l$(ARCH_LIBNAME)_dis -l$(ARCH_LIBNAME)_gdc\
#							-lhi_sensor_i2c -lhi_sensor_spi\
#							-l$(ARCH_LIBNAME)_ai -l$(ARCH_LIBNAME)_ao -l$(ARCH_LIBNAME)_aio -l$(ARCH_LIBNAME)_aenc -l$(ARCH_LIBNAME)_adec -l$(ARCH_LIBNAME)_acodec\
#							-lhi_sensor_spi -lhi_pwm -lhi_piris \
#							-l$(ARCH_LIBNAME)_ive -l$(ARCH_LIBNAME)_nnie -l$(ARCH_LIBNAME)_vfmw -l$(ARCH_LIBNAME)_vfmw -l$(ARCH_LIBNAME)_cipher\
#							--end-group


#SDK_LIB += $(SENSOR_LIBS)

LITEOS_LIBDEPS = --start-group $(LITEOS_LIBDEP) --end-group $(LITEOS_TABLES_LDFLAGS)

LDFLAGS := -L$(OUTDIR)/obj/kernel/liteos_a/lib $(LITEOS_LDFLAGS) --gc-sections

CFLAGS += -I $(LITEOSTOPDIR)/../../$(LOSCFG_BOARD_CONFIG_PATH)/include

# target source
ifeq ($(LOSCFG_COMPILER_CLANG_LLVM), y)
SRCS  := $(MPP_DIR)/src/system_init.c
SRCS  += $(MPP_DIR)/src/sdk_init.c
else ifeq ($(LOSCFG_COMPILER_HIMIX_32), y)
SRCS  := $(MPP_DIR)/init/OHOS/system_init.c
SRCS  += $(MPP_DIR)/init/OHOS/sdk_init.c
else
$(error  "########################compiler is not supported################################")
endif
#SRCS  := $(MPP_DIR)/src/system_init.c
#SRCS  += $(MPP_DIR)/src/sdk_init.c
OBJS  := $(SRCS:%.c=%.o)
OBJS += $(COMM_OBJ)

BIN := $(TARGET_PATH)/$(TARGET).bin
MAP := $(TARGET_PATH)/$(TARGET).map

all: $(BIN)

$(BIN):$(TARGET)
	@$(OBJCOPY) -O binary $(TARGET_PATH)/$(TARGET) $(BIN)
	cp $(TARGET_PATH)/$(TARGET)* $(OUTDIR)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -Map=$(MAP) -o $(TARGET_PATH)/$(TARGET) $(OBJS) $(SDK_LIB) $(LITEOS_LIBDEPS)
	@$(OBJDUMP) -d $(TARGET_PATH)/$(TARGET) > $(TARGET_PATH)/$(TARGET).asm

$(OBJS):%.o:%.c
	@$(CC) $(CFLAGS) $(LITEOS_CFLAGS) -c $< -o $@

clean:
	@rm -f $(TARGET_PATH)/$(TARGET) $(BIN) $(MAP) $(TARGET_PATH)/*.asm
	@rm -f $(OBJS)

.PHONY : clean all

