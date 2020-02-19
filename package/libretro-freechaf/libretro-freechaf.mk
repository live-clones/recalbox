################################################################################
#
# FREECHAF
#
################################################################################

LIBRETRO_FREECHAF_VERSION = 8e46bf0cfb69fd7b3627f60dcadc96faaaad1fe6
LIBRETRO_FREECHAF_SITE = $(call github,libretro,FreeChaF,$(LIBRETRO_FREECHAF_VERSION))

define LIBRETRO_FREECHAF_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	# Currently, FreeChaF crash when compiled with LTO.
	# As it might be a compiler or a code issue, try to reactivate LTO
	# on next toolchain or code bump.
	#$(SED) "s|float time = 0.0;|static float time = 0.0;|g" $(@D)/src/audio.c
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_FREECHAF_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/freechaf_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/freechaf_libretro.so
endef

$(eval $(generic-package))
