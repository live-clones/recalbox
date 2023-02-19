################################################################################
#
# PARALLEL-N64
#
################################################################################

# Version: 2022-05-12
LIBRETRO_PARALLEL_N64_VERSION = 1922f4a60bb1335b0df995dcc0c232d82a32bb2d
LIBRETRO_PARALLEL_N64_SITE = $(call github,libretro,parallel-n64,$(LIBRETRO_PARALLEL_N64_VERSION))
LIBRETRO_PARALLEL_N64_LICENSE = GPL-3.0

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
LIBRETRO_PARALLEL_N64_DEPENDENCIES += rpi-userland
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
LIBRETRO_PARALLEL_N64_DEPENDENCIES += libgles
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
LIBRETRO_PARALLEL_N64_DEPENDENCIES += libgl
endif

ifeq ($(BR2_i386)$(BR2_x86_64),y)
LIBRETRO_PARALLEL_N64_DEPENDENCIES += host-nasm
endif

# LTO switched off on PC platforms
# Platform dependent options

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2LEGACY),y)
LIBRETRO_PARALLEL_N64_PLATFORM=unix-rpi3
LIBRETRO_PARALLEL_N64_DYNAREC=arm
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2),y)
LIBRETRO_PARALLEL_N64_PLATFORM=unix-rpi3-mesa
LIBRETRO_PARALLEL_N64_DYNAREC=arm
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
LIBRETRO_PARALLEL_N64_PLATFORM=unix-rpi3-mesa
LIBRETRO_PARALLEL_N64_DYNAREC=arm
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
LIBRETRO_PARALLEL_N64_PLATFORM=unix-rpi4_64-mesa
LIBRETRO_PARALLEL_N64_SUPP_OPT=HAVE_OPENGL=1 GLES=1 GL_LIB:=-lGLESv2
LIBRETRO_PARALLEL_N64_DYNAREC=aarch64
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_PARALLEL_N64_PLATFORM=odroid
LIBRETRO_PARALLEL_N64_SUPP_OPT=BOARD=ODROID-XU
LIBRETRO_PARALLEL_N64_DYNAREC=arm
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
LIBRETRO_PARALLEL_N64_PLATFORM = odroid
LIBRETRO_PARALLEL_N64_SUPP_OPT = BOARD=ODROIDGO2 HAVE_PARELLEL_RSP=1 LIBS=-lmali
LIBRETRO_PARALLEL_N64_DYNAREC = aarch64
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_i386),y)
LIBRETRO_PARALLEL_N64_SUPP_OPT=ARCH=i686 HAVE_PARALLEL_RSP=1 HAVE_THR_AL=1
LIBRETRO_PARALLEL_N64_PLATFORM=unix
LIBRETRO_PARALLEL_N64_DYNAREC=x86
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else ifeq ($(BR2_x86_64),y)
LIBRETRO_PARALLEL_N64_SUPP_OPT=ARCH=x86_64 HAVE_PARALLEL_RSP=1 HAVE_THR_AL=1
LIBRETRO_PARALLEL_N64_PLATFORM=unix
LIBRETRO_PARALLEL_N64_DYNAREC=x86_64
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

else
LIBRETRO_PARALLEL_N64_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_PARALLEL_N64_SUPP_OPT=
LIBRETRO_PARALLEL_N64_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_PARALLEL_N64_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_PARALLEL_N64_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif

define LIBRETRO_PARALLEL_N64_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	PLATCFLAGS="$(TARGET_CFLAGS)" CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_PARALLEL_N64_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_PARALLEL_N64_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_PARALLEL_N64_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile \
		platform="$(LIBRETRO_PARALLEL_N64_PLATFORM)" $(LIBRETRO_PARALLEL_N64_SUPP_OPT) \
		WITH_DYNAREC=$(LIBRETRO_PARALLEL_N64_DYNAREC)
		# HAVE_PARALLEL=1 HAVE_PARALLEL_RSP=1
endef

define LIBRETRO_PARALLEL_N64_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/parallel_n64_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/parallel_n64_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/64dd
endef

$(eval $(generic-package))
