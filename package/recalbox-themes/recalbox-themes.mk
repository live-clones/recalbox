################################################################################
#
# Recalbox themes for EmulationStation : https://gitlab.com/recalbox/recalbox-themes
#
################################################################################

ifneq ($(GITLAB_TOKEN_THEMES),)
RECALBOX_THEMES_VERSION = f16544ce1ce3b19a585319836ce41e84ec3c17f9
RECALBOX_THEMES_SITE = https://gitlab-ci-token:$(GITLAB_TOKEN_THEMES)@gitlab.com/recalbox/recalbox-themes-prime
else
RECALBOX_THEMES_VERSION = dd10b43c5bf48861274a77af9e604b1fa5bcc2ae
RECALBOX_THEMES_SITE = https://gitlab.com/recalbox/recalbox-themes
endif
RECALBOX_THEMES_SITE_METHOD = git
RECALBOX_THEMES_LICENSE = CC-BY-NC-ND-4.0

define RECALBOX_THEMES_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
	cp -r $(@D)/themes/recalbox-next \
		$(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
	cp -r $(@D)/themes/recalbox-goa2 \
		$(TARGET_DIR)/recalbox/share_init/system/.emulationstation/themes/
endef

$(eval $(generic-package))
