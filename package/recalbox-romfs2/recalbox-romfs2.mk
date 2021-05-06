################################################################################
#
# recalbox romfs 2
#
################################################################################

RECALBOX_ROMFS2_LICENSE = MIT
RECALBOX_ROMFS2_DEPENDECIES = host-python3

define RECALBOX_ROMFS2_BUILD_CMDS
	python3 $(RECALBOX_ROMFS2_PKGDIR)/scripts/romfs.py -buildsystems $(@D)/es_systems.cfg
endef

define RECALBOX_ROMFS2_INSTALL_TARGET_CMDS
	cp $(@D)/es_systems.cfg $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/
	python3 $(@D)/romfs.py -installroms $(TARGET_DIR)/recalbox/share_init/roms/
endef

$(eval $(generic-package))
