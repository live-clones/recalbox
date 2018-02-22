################################################################################
#
# Recalbox themes for EmulationStation : https://gitlab.com/recalbox/recalbox-themes
#
################################################################################

RECALBOX_THEMES_VERSION = 90a90c4f661e7864b881eb1f0899266b25264cbc
RECALBOX_THEMES_SITE = https://gitlab.com/recalbox/recalbox-themes.git
RECALBOX_THEMES_SITE_METHOD = git

define RECALBOX_THEMES_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
	cp -r $(@D)/themes/recalbox-next \
		$(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
endef

$(eval $(generic-package))
