################################################################################
#
# SAMEBOY
#
################################################################################

LIBRETRO_SAMEBOY_VERSION = fc754112163a2f6157c56092ea6e1fd64500c4cc
LIBRETRO_SAMEBOY_SITE = $(call github,libretro,SameBoy,$(LIBRETRO_SAMEBOY_VERSION))
# host-util-linux is required because sameboy's makefile uses hexdump
LIBRETRO_SAMEBOY_DEPENDENCIES = host-rgbds host-util-linux

define LIBRETRO_SAMEBOY_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	# Haha... when using -Werror you should ensure there are actually *NO* warning at all :)
	$(SED) "s|-Werror||g" $(@D)/Makefile
	$(MAKE) CC="$(HOSTCC)" -C $(@D) build/logo-compress
	$(TARGET_MAKE_ENV) CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" -C $(@D) platform="unix" libretro
	$(TARGET_MAKE_ENV) CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" -C $(@D) platform="unix" bootroms
endef

define LIBRETRO_SAMEBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/build/bin/sameboy_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/sameboy_libretro.so
	$(INSTALL) -D $(@D)/build/bin/BootROMs/*.bin \
		$(TARGET_DIR)/recalbox/share_init/bios/
endef

$(eval $(generic-package))
