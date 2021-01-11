################################################################################
#
# guichan
#
################################################################################

GUICHAN_VERSION = 0.8.2
GUICHAN_SITE = https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/guichan
GUICHAN_DEPENDENCIES = sdl sdl_image
GUICHAN_INSTALL_STAGING = YES
GUICHAN_LICENSE = Custom
GUICHAN_LICENSE_FILES = COPYING

define GUICHAN_FIX_SDL_CONFIG_PATH
	$(SED) "s+sdl-config+$(STAGING_DIR)/usr/bin/sdl-config+g" $(@D)/configure
endef

GUICHAN_PRE_CONFIGURE_HOOKS += GUICHAN_FIX_SDL_CONFIG_PATH
GUICHAN_CONF_OPTS += --enable-force-sdl

$(eval $(autotools-package))
