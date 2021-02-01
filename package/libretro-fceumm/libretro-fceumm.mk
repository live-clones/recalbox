################################################################################
#
# FCEUMM
#
################################################################################

LIBRETRO_FCEUMM_VERSION = 38c75ada8dbbcc17d412eb9fa92e1bc9f1641939
LIBRETRO_FCEUMM_SITE = $(call github,libretro,libretro-fceumm,$(LIBRETRO_FCEUMM_VERSION))
LIBRETRO_FCEUMM_LICENSE = GPL-2.0
LIBRETRO_FCEUMM_LICENSE_FILES = Copying

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_FCEUMM_PLATFORM=rpi2
else
LIBRETRO_FCEUMM_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
endif

define LIBRETRO_FCEUMM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile.libretro platform="$(LIBRETRO_FCEUMM_PLATFORM)"
endef

define LIBRETRO_FCEUMM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/fceumm_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/fceumm_libretro.so
endef

$(eval $(generic-package))
