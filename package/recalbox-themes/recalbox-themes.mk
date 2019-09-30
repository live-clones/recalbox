################################################################################
#
# Recalbox themes for EmulationStation : https://gitlab.com/recalbox/recalbox-themes
#
################################################################################

RECALBOX_THEMES_VERSION = a76cca586c98879471d90bbaf7f0aeffc57da6ba
RECALBOX_THEMES_SITE = https://gitlab.com/recalbox/recalbox-themes
RECALBOX_THEMES_SITE_METHOD = git

define RECALBOX_THEMES_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
	cp -r $(@D)/themes/recalbox-next \
		$(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
endef

$(eval $(generic-package))
