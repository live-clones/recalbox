################################################################################
#
# amiberry
#
################################################################################

AMIBERRY_VERSION = 2.1
AMIBERRY_SITE = $(call github,midwan,amiberry,v$(AMIBERRY_VERSION))
AMIBERRY_DEPENDENCIES = sdl sdl_image guichan mpg123 host-pkgconf sdl_ttf sdl_gfx
#~ AMIBERRY_VERSION = dev
#~ AMIBERRY_SITE = $(call github,midwan,amiberry,$(AMIBERRY_VERSION))
#~ AMIBERRY_DEPENDENCIES = sdl sdl_image guichan mpg123 host-pkgconf sdl2_ttf

AMIBERRY_PLATFORM = $(RECALBOX_SYSTEM_VERSION)

define AMIBERRY_RIGHT_PI_PATH
	$(SED) "s+-I/opt/vc+-I$(STAGING_DIR)/usr+g" $(@D)/Makefile
	$(SED) "s+-Isrc/include+-Isrc/include -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL+g" $(@D)/Makefile
	$(SED) "s+-L/opt/vc/lib+-L$(STAGING_DIR)/usr/lib -lvchostif -lbcm_host `$(STAGING_DIR)/usr/bin/sdl-config --libs`+g" $(@D)/Makefile
	$(SED) "s+xml2-config+$(STAGING_DIR)/usr/bin/xml2-config+g" $(@D)/Makefile
	$(SED) "s+sdl-config+$(STAGING_DIR)/usr/bin/sdl-config+g" $(@D)/Makefile
	# For sdl2-dev branch
#~ 	$(SED) "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/Makefile
#~ 	$(SED) "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/src/guisan/Makefile
endef
AMIBERRY_PRE_CONFIGURE_HOOKS += AMIBERRY_RIGHT_PI_PATH

define AMIBERRY_BUILD_CMDS
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D) \
		-f Makefile \
		PLATFORM="$(AMIBERRY_PLATFORM)"
endef

define AMIBERRY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/amiberry $(TARGET_DIR)/usr/bin
	cp -R $(@D)/data $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
