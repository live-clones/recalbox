################################################################################
#
# VITAQUAKE3
#
################################################################################

LIBRETRO_VITAQUAKE3_VERSION = 7d9512b9a1e078a060a882b4efaeb45c810ff34e
LIBRETRO_VITAQUAKE3_SITE = $(call github,libretro,vitaquake3,$(LIBRETRO_VITAQUAKE3_VERSION))
LIBRETRO_VITAQUAKE3_LICENSE = GPL-2.0
LIBRETRO_VITAQUAKE3_LICENSE_FILES = COPYING.txt

define LIBRETRO_VITAQUAKE3_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_VITAQUAKE3_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vitaquake3_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vitaquake3_libretro.so
endef

$(eval $(generic-package))
