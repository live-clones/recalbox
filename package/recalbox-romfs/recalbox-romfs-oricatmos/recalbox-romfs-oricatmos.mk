################################################################################
#
# recalbox-romfs-oricatmos
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system oricatmos --extension '.dsk .DSK .tap .TAP' --fullname 'Oric/Atmos' --platform oricatmos --theme oricatmos BR2_PACKAGE_ORICUTRON

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
define CONFIGURE_ORICATMOS
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_ORICATMOS),Oric/Atmos,$(SYSTEM_NAME_ORICATMOS),.dsk .DSK .tap .TAP,oricatmos,oricatmos,$(SOURCE_ROMDIR_ORICATMOS),$(@D))
endef
RECALBOX_ROMFS_ORICATMOS_CONFIGURE_CMDS += $(CONFIGURE_ORICATMOS)
endif

$(eval $(generic-package))
