################################################################################
#
# FLYCAST
#
################################################################################

LIBRETRO_FLYCAST_VERSION = 04d45ec7f69538e57303e48f66dabcd90b588851
LIBRETRO_FLYCAST_SITE = $(call github,libretro,flycast,$(LIBRETRO_FLYCAST_VERSION))
LIBRETRO_FLYCAST_LICENSE = GPLv2

LIBRETRO_FLYCAST_PLATFORM = $(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_FLYCAST_SUPP_OPT =

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
LIBRETRO_FLYCAST_SUPP_OPT += ARCH=x86 FORCE_GLES=1 LDFLAGS=-lrt
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
LIBRETRO_FLYCAST_SUPP_OPT += ARCH=x86_64 LDFLAGS=-lrt
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_FLYCAST_PLATFORM = odroid
LIBRETRO_FLYCAST_SUPP_OPT += BOARD=ODROID-XU4 ARCH=arm FORCE_GLES=1 CC_AS="$(TARGET_CC)"
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_FLYCAST_PLATFORM = rpi3
LIBRETRO_FLYCAST_SUPP_OPT += ARCH=arm FORCE_GLES=1 CC_AS="$(TARGET_CC)" LDFLAGS=-lrt
endif

LIBRETRO_FLYCAST_MAKE_ARGS = platform="$(LIBRETRO_FLYCAST_PLATFORM)" $(LIBRETRO_FLYCAST_SUPP_OPT)

define LIBRETRO_FLYCAST_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/ -f Makefile $(LIBRETRO_FLYCAST_MAKE_ARGS)
endef

define LIBRETRO_FLYCAST_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/flycast_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/flycast_libretro.so
endef

$(eval $(generic-package))
