################################################################################
#
# SAMEBOY
#
################################################################################

LIBRETRO_SAMEBOY_VERSION = 44d8b16c107c79205331c53c93c1f25386e88bf8
LIBRETRO_SAMEBOY_SITE = $(call github,libretro,SameBoy,$(LIBRETRO_SAMEBOY_VERSION))
LIBRETRO_SAMEBOY_LICENSE = MIT
LIBRETRO_SAMEBOY_LICENSE_FILES = LICENSE

define LIBRETRO_SAMEBOY_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro -f Makefile platform="unix"
endef

define LIBRETRO_SAMEBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/sameboy_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/sameboy_libretro.so
	$(INSTALL) -D $(@D)/BootROMs/prebuilt/*.bin \
		$(TARGET_DIR)/recalbox/share_upgrade/bios/
endef

$(eval $(generic-package))
