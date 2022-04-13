TOP_DIR = ../..
sinclude $(TOP_DIR)/tools/w800/conf.mk

ifeq ($(USE_LITEOS),true)
ifndef PDIR
GEN_LIBS = libos$(LIB_EXT)
COMPONENTS_libos = liteos/libliteos$(LIB_EXT)
endif
else
ifndef PDIR
GEN_LIBS = libos$(LIB_EXT)
COMPONENTS_libos = rtos/librtos$(LIB_EXT)
endif
endif


#DEFINES +=

sinclude $(TOP_DIR)/tools/w800/rules.mk

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
