################################################################################
#
# YABASANSHIRO
#
################################################################################

# https://github.com/libretro/yabause/tree/yabasanshiro/yabause/src/libretro
LIBRETRO_YABASANSHIRO_VERSION = 7ae0de7abc378f6077aff0fd365ab25cff58b055
LIBRETRO_YABASANSHIRO_SITE = $(call github,libretro,yabause,$(LIBRETRO_YABASANSHIRO_VERSION))

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
LIBRETRO_YABASANSHIRO_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_YABASANSHIRO_SUPP_OPT=FORCE_GLES=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_YABASANSHIRO_PLATFORM=odroid
LIBRETRO_YABASANSHIRO_SUPP_OPT=BOARD="ODROID-XU4" FORCE_GLES=1
else
LIBRETRO_YABASANSHIRO_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_YABASANSHIRO_SUPP_OPT=
endif

define LIBRETRO_YABASANSHIRO_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/yabause/src/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" LD="$(TARGET_LD)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/yabause/src/libretro -f Makefile platform="$(LIBRETRO_YABASANSHIRO_PLATFORM)" $(LIBRETRO_YABASANSHIRO_SUPP_OPT)
endef

define LIBRETRO_YABASANSHIRO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/yabause/src/libretro/yabasanshiro_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/yabasanshiro_libretro.so
endef

$(eval $(generic-package))
