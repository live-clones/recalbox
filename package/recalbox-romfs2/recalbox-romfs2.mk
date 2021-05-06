################################################################################
#
# recalbox romfs 2
#
################################################################################

RECALBOX_ROMFS2_LICENSE = MIT
RECALBOX_ROMFS2_DEPENDECIES = host-python3 recalbox-romfs

define RECALBOX_ROMFS2_BUILD_CMDS
	$(HOST_DIR)/usr/bin/python3 $(RECALBOX_ROMFS2_PKGDIR)/scripts/romfs.py \
		-systems $(RECALBOX_ROMFS2_PKGDIR)/systems -buildsystems $(@D)/es_systems.cfg \
		-mergefrom $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/es_systems.cfg
endef

define RECALBOX_ROMFS2_INSTALL_TARGET_CMDS
	cp $(@D)/es_systems.cfg $(TARGET_DIR)/recalbox/share_init/system/.emulationstation/
	$(HOST_DIR)/usr/bin/python3 $(RECALBOX_ROMFS2_PKGDIR)/scripts/romfs.py \
		-systems $(RECALBOX_ROMFS2_PKGDIR)/systems -installroms $(TARGET_DIR)/recalbox/share_init/roms/
endef

$(eval $(generic-package))
