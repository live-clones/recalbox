################################################################################
#
# SAMEBOY
#
################################################################################

LIBRETRO_SAMEBOY_VERSION = fc754112163a2f6157c56092ea6e1fd64500c4cc
LIBRETRO_SAMEBOY_SITE = $(call github,libretro,SameBoy,$(LIBRETRO_SAMEBOY_VERSION))
LIBRETRO_SAMEBOY_DEPENDENCIES = rgbds

#define LIBRETRO_SAMEBOY_PATH_TO_RGBDS
#	# Missing variable definition to  rgb* tools - Use a brutal SED for now
#	$(SED) "s|rgbasm|$(HOST_DIR)/usr/bin/rgbasm|g" $(@D)/Makefile
#	$(SED) "s|rgblink|$(HOST_DIR)/usr/bin/rgblink|g" $(@D)/Makefile
#	$(SED) "s|rgbgfx|$(HOST_DIR)/usr/bin/rgbgfx|g" $(@D)/Makefile
#endef
#LIBRETRO_SAMEBOY_POST_EXTRACT_HOOKS += LIBRETRO_SAMEBOY_PATH_TO_RGBDS

define LIBRETRO_SAMEBOY_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	# Haha... when using -Werror you should ensure there are actually *NO* warning at all :)
	$(SED) "s|-Werror||g" $(@D)/Makefile
	$(MAKE) HOST_CC="gcc" -C $(@D) logo-compress
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		RGBDS_DIR="$(STAGING_DIR)/usr/bin" \
		$(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" -C $(@D) platform="unix" bootroms
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		RGBDS_DIR="$(STAGING_DIR)/usr/bin" \
		$(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" -C $(@D) platform="unix" libretro
endef

define LIBRETRO_SAMEBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/build/bin/sameboy_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/sameboy_libretro.so
	$(INSTALL) -m 755 $(@D)/build/bin/BootROMs/*.bin \
		$(TARGET_DIR)/recalbox/share_init/bios/
endef

$(eval $(generic-package))
