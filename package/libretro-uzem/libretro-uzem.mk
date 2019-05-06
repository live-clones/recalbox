################################################################################
#
# UzeBox
#
################################################################################
LIBRETRO_UZEM_VERSION = 3c1988cc0e02c8dd389130282a3b2db8d9c3d6c0
LIBRETRO_UZEM_SITE = $(call github,t-paul,uzebox,$(LIBRETRO_UZEM_VERSION))

define LIBRETRO_UZEM_FIX_MISSING_PATH
	mkdir -p $(@D)/tools/uzem/Release
endef

LIBRETRO_UZEM_PRE_BUILD_HOOKS += LIBRETRO_UZEM_FIX_MISSING_PATH

define LIBRETRO_UZEM_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -fmerge-all-constants -lstdc++ -lm" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/tools/uzem -f Makefile release OS=LINUX NOGDB=1 LIBRETRO_BUILD=1
endef

define LIBRETRO_UZEM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/tools/uzem/uzem_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/uzem_libretro.so
endef

$(eval $(generic-package))
