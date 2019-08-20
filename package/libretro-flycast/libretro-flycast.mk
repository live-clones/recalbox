################################################################################
#
# FLYCAST
#
################################################################################

LIBRETRO_FLYCAST_VERSION = ceda229e3b742411aef5cd8d5564f0faa137fce8
LIBRETRO_FLYCAST_SITE = $(call github,libretro,flycast,$(LIBRETRO_FLYCAST_VERSION))
LIBRETRO_FLYCAST_LICENSE = GPLv2

LIBRETRO_FLYCAST_PLATFORM = $(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_FLYCAST_BOARD = $(RETROARCH_LIBRETRO_BOARD)
ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_FLYCAST_PLATFORM = odroid
LIBRETRO_FLYCAST_BOARD = ODROID-XU4
endif

LIBRETRO_FLYCAST_MAKE_ARGS = HAVE_OPENMP=0 FORCE_GLES=1 platform="$(LIBRETRO_FLYCAST_PLATFORM)" BOARD="$(LIBRETRO_FLYCAST_BOARD)"

define LIBRETRO_FLYCAST_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC_AS="$(TARGET_CC)" CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/ -f Makefile $(LIBRETRO_FLYCAST_MAKE_ARGS)
endef

define LIBRETRO_FLYCAST_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/flycast_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/flycast_libretro.so
endef

$(eval $(generic-package))
