################################################################################
#
# UzeBox
#
################################################################################

LIBRETRO_UZEM_VERSION = fdecf52e6b7927c9d3f3bce6aa78db16d45480ef
LIBRETRO_UZEM_SITE = $(call github,libretro,libretro-uzem,$(LIBRETRO_UZEM_VERSION))

define LIBRETRO_UZEM_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -fmerge-all-constants -lstdc++ -lm" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile
endef

define LIBRETRO_UZEM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/uzem_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/uzem_libretro.so
endef

$(eval $(generic-package))
