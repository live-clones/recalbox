################################################################################
#
# XMIL
#
################################################################################

LIBRETRO_XMIL_VERSION = b07506c0cae31d260db28cb079148857d6ca2e93
LIBRETRO_XMIL_SITE = $(call github,r-type,xmil-libretro,$(LIBRETRO_XMIL_VERSION))

define LIBRETRO_XMIL_BUILD_CMDS
	$(SED) "s|--export-all-symbols||g"
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_XMIL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/x1_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/xmil_libretro.so
endef

$(eval $(generic-package))
