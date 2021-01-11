################################################################################
#
# VITAVOYAGER
#
################################################################################

LIBRETRO_VITAVOYAGER_VERSION = 58b70dd7ea891c2e8ddc37d03a5d58116b0ea118
LIBRETRO_VITAVOYAGER_SITE = $(call github,libretro,vitaVoyager,$(LIBRETRO_VITAVOYAGER_VERSION))
LIBRETRO_VITAVOYAGER_LICENSE = GPL-2.0
LIBRETRO_VITAVOYAGER_LICENSE_FILES = COPYING.txt

define LIBRETRO_VITAVOYAGER_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_VITAVOYAGER_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vitavoyager_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vitavoyager_libretro.so
endef

$(eval $(generic-package))
