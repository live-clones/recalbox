################################################################################
#
# RACE
#
################################################################################

LIBRETRO_RACE_VERSION = 7ad607340bfc188ed819e15386366c4cd890cc18
LIBRETRO_RACE_SITE = $(call github,libretro,RACE,$(LIBRETRO_RACE_VERSION))
LIBRETRO_RACE_LICENSE = GPL-2.0
LIBRETRO_RACE_LICENSE_FILES = license.txt

define LIBRETRO_RACE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_RACE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/race_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/race_libretro.so
endef

$(eval $(generic-package))
