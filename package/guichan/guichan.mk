################################################################################
#
# guichan
#
#################################################################################

GUICHAN_VERSION = 0.8.2
GUICHAN_SOURCE = guichan-$(GUICHAN_VERSION).tar.gz
GUICHAN_SITE = https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/guichan
#~ GUICHAN_VERSION = 1a727941539e7ed4376dc8194cb4988961c86340
#~ GUICHAN_SITE = $(call github,sphaero,guichan,$(GUICHAN_VERSION))
GUICHAN_DEPENDENCIES = sdl sdl_image
GUICHAN_INSTALL_STAGING = YES

define GUICHAN_FIX_SDL_CONFIG_PATH
	$(SED) "s+sdl-config+$(STAGING_DIR)/usr/bin/sdl-config+g" $(@D)/configure
endef

GUICHAN_PRE_CONFIGURE_HOOKS += GUICHAN_FIX_SDL_CONFIG_PATH
GUICHAN_CONF_OPTS += --enable-force-sdl

$(eval $(autotools-package))
