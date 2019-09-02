################################################################################
#
# SCUMMVM
#
################################################################################

LIBRETRO_SCUMMVM_VERSION = e07a6ede61c364fb87630fa7507a4f8482d882e0
LIBRETRO_SCUMMVM_SITE = $(call github,libretro,scummvm,$(LIBRETRO_SCUMMVM_VERSION))

define LIBRETRO_SCUMMVM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/backends/platform/libretro/build/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO) -shared -Wl,--no-undefined" \
		$(MAKE) TOOLSET="$(TARGET_CROSS)" -C $(@D)/backends/platform/libretro/build platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_SCUMMVM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/backends/platform/libretro/build/scummvm_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/scummvm_libretro.so
endef

$(eval $(generic-package))
