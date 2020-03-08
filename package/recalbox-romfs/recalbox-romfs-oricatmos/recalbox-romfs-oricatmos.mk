################################################################################
#
# recalbox-romfs-oricatmos
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system oricatmos --extension '.dsk .DSK .tap .TAP' --fullname 'Oric/Atmos' --platform oricatmos --theme oric 1:oricutron:oricutron:BR2_PACKAGE_ORICUTRON

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ORICATMOS_SOURCE = 
RECALBOX_ROMFS_ORICATMOS_SITE = 
RECALBOX_ROMFS_ORICATMOS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ORICATMOS = oricatmos
SYSTEM_XML_ORICATMOS = $(@D)/$(SYSTEM_NAME_ORICATMOS).xml
# System rom path
SOURCE_ROMDIR_ORICATMOS = $(RECALBOX_ROMFS_ORICATMOS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_ORICUTRON),)
define CONFIGURE_MAIN_ORICATMOS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ORICATMOS),Oric/Atmos,$(SYSTEM_NAME_ORICATMOS),.dsk .DSK .tap .TAP,oricatmos,oric)
endef

ifneq ($(BR2_PACKAGE_ORICUTRON),)
define CONFIGURE_ORICATMOS_ORICUTRON_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ORICATMOS),oricutron)
endef
ifeq ($(BR2_PACKAGE_ORICUTRON),y)
define CONFIGURE_ORICATMOS_ORICUTRON_ORICUTRON_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ORICATMOS),oricutron,1)
endef
endif

define CONFIGURE_ORICATMOS_ORICUTRON_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ORICATMOS))
endef
endif



define CONFIGURE_MAIN_ORICATMOS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ORICATMOS),$(SOURCE_ROMDIR_ORICATMOS),$(@D))
endef
endif

define RECALBOX_ROMFS_ORICATMOS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ORICATMOS_START)
	$(CONFIGURE_ORICATMOS_ORICUTRON_START)
	$(CONFIGURE_ORICATMOS_ORICUTRON_ORICUTRON_DEF)
	$(CONFIGURE_ORICATMOS_ORICUTRON_END)
	$(CONFIGURE_MAIN_ORICATMOS_END)
endef

$(eval $(generic-package))
