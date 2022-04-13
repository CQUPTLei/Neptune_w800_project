TOP_DIR = ../../..
sinclude $(TOP_DIR)/tools/w800/conf.mk
ifndef PDIR
GEN_LIBS = libliteos$(LIB_EXT)
# COMPONENTS_libliteos = kernel/libkernel$(LIB_EXT)
# COMPONENTS_libliteos += bounds_checking_function/libbcf$(LIB_EXT)
# COMPONENTS_libliteos += arch/libck804$(LIB_EXT)
#COMPONENTS_libliteos += platform/libplatform$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/w800/rules.mk

INCLUDES := $(INCLUDES) -I ./ -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
