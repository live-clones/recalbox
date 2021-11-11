################################################################################
#
# YABASANSHIRO
#
################################################################################

# https://github.com/libretro/yabause/tree/yabasanshiro/yabause/src/libretro
LIBRETRO_YABASANSHIRO_VERSION = db67d16c89f4c10f958a0ae72209d6651111007c
LIBRETRO_YABASANSHIRO_SITE = $(call github,libretro,yabause,$(LIBRETRO_YABASANSHIRO_VERSION))
LIBRETRO_YABASANSHIRO_LICENSE = GPL-2.0
LIBRETRO_YABASANSHIRO_DEPENDENCIES = host-dos2unix

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_YABASANSHIRO_PLATFORM=odroid
LIBRETRO_YABASANSHIRO_SUPP_OPT=BOARD="ODROID-XU4" FORCE_GLES=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
LIBRETRO_YABASANSHIRO_PLATFORM=odroid-go2
LIBRETRO_YABASANSHIRO_SUPP_OPT=
LIBRETRO_YABASANSHIRO_LDFLAGS_OPT = -lGLESv2 -lmali -lpthread
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
LIBRETRO_YABASANSHIRO_PLATFORM=unix-armv
LIBRETRO_YABASANSHIRO_SUPP_OPT=FORCE_GLES=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
LIBRETRO_YABASANSHIRO_PLATFORM=arm64
LIBRETRO_YABASANSHIRO_SUPP_OPT=FORCE_GLES=1
else
LIBRETRO_YABASANSHIRO_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_YABASANSHIRO_SUPP_OPT=
endif

define LIBRETRO_YABASANSHIRO_DOSUNIX_PATCH
	$(HOST_DIR)/usr/bin/dos2unix $(@D)/yabause/src/yglshaderes.c
endef

LIBRETRO_YABASANSHIRO_PRE_PATCH_HOOKS += LIBRETRO_YABASANSHIRO_DOSUNIX_PATCH

define LIBRETRO_YABASANSHIRO_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/yabause/src/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO) $(LIBRETRO_YABASANSHIRO_LDFLAGS_OPT)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" LD="$(TARGET_LD)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/yabause/src/libretro -f Makefile platform="$(LIBRETRO_YABASANSHIRO_PLATFORM)" $(LIBRETRO_YABASANSHIRO_SUPP_OPT)
endef

define LIBRETRO_YABASANSHIRO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/yabause/src/libretro/yabasanshiro_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/yabasanshiro_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/saturn
endef

$(eval $(generic-package))
