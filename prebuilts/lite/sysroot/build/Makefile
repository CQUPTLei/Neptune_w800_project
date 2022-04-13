include $(musldir)/config.mak

HIDE := @

optdir = ./optimized-routines

MUSL_ALL_OBJS = $(sort $(wildcard $(musldir)/obj/src/*/*.lo) $(wildcard $(musldir)/obj/src/*/$(ARCH)/*.lo) $(wildcard $(musldir)/obj/ldso/*.lo) $(wildcard $(musldir)/obj/compat/time32/*.lo))
MUSL_LIBC_OBJS = $(filter $(musldir)/obj/src/%,$(MUSL_ALL_OBJS)) $(filter $(musldir)/obj/compat/%,$(MUSL_ALL_OBJS))

REPLACED_FUNC = memchr memcpy memcpy_le strcmp strcpy strlen
REPLACED_OBJS = $(addprefix $(musldir)/obj/src/string/,$(addsuffix .lo,$(REPLACED_FUNC)))
REPLACED_OBJS += $(addprefix $(musldir)/obj/src/string/$(ARCH)/,$(addsuffix .lo,$(REPLACED_FUNC)))
MUSL_LIBC_OBJS := $(filter-out $(REPLACED_OBJS),$(MUSL_LIBC_OBJS))
MUSL_LDSO_OBJS = $(filter $(musldir)/obj/ldso/%,$(MUSL_ALL_OBJS:%.o=%.lo))

OPT_ALL_OBJS = $(sort $(wildcard $(optdir)/build/string/arm/*.o))
OPT_AOBJS = $(OPT_ALL_OBJS)
OPT_OBJS = $(filter $(optdir)/build/string/arm/%,$(OPT_ALL_OBJS:%.o=%.os))

LDFLAGS_ALL = -Wl,--sort-section,alignment -Wl,--sort-common -Wl,--gc-sections -Wl,--hash-style=both -Wl,--no-undefined -Wl,--exclude-libs=ALL -Wl,--dynamic-list=$(musldir)/dynamic.list
LDFLAGS_ALL += $(LDFLAGS)

ifeq ("$(CROSS_COMPILE)", "llvm-")
	TARGET_LIBC_A = libc_opt_clang.a
	TARGET_LIBC_SO = libc_opt_clang.so
else
	TARGET_LIBC_A = libc_opt.a
	TARGET_LIBC_SO = libc_opt.so
endif

ifeq ("$(ARCH)","")

all:
	@echo "Please run linux_header_install.sh thirdparty_headers.sh build_musl(_clang).sh before running make."
	@exit 1

else

ifeq ("$(ARCH)","arm")
# making a new libc.a libc.so
all: $(TARGET_LIBC_A) $(TARGET_LIBC_SO)
	@echo "success!"

$(TARGET_LIBC_A):
	$(HIDE) $(AR) rc $@ $(MUSL_LIBC_OBJS) $(OPT_AOBJS)
	$(HIDE) $(RANLIB) $@

$(TARGET_LIBC_SO):
	$(HIDE) $(CC) $(LDFLAGS_ALL) -L ${musldir}/lib/ -ldl -lpthread -nostdlib -shared -Wl,-e,_dlstart -o $@ $(MUSL_LIBC_OBJS) $(OPT_OBJS) $(MUSL_LDSO_OBJS) $(LIBCC)

clean:
ifeq ($(TARGET_LIBC_A), $(wildcard $(TARGET_LIBC_A)))
	rm $(TARGET_LIBC_A)
endif
ifeq ($(TARGET_LIBC_SO), $(wildcard $(TARGET_LIBC_SO)))
	rm $(TARGET_LIBC_SO)
endif
endif
endif
