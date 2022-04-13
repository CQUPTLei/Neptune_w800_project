TOP_DIR = ../..
sinclude $(TOP_DIR)/tools/w800/conf.mk

ifndef PDIR
GEN_LIBS = libwmsys$(LIB_EXT)
endif

#DEFINES +=
ifeq ($(USE_DEMO_APP), true)
DEFINES += -DUSE_DEMO_APP
endif

sinclude $(TOP_DIR)/tools/w800/rules.mk
INCLUDES := $(INCLUDES) -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile