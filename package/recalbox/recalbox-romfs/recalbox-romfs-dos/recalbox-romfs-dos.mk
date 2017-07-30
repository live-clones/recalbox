################################################################################
#
# recalbox-romfs-dos
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system dos --extension '.pc .PC .dos .DOS' --fullname 'Dos (x86)' --platform pc --theme pc BR2_PACKAGE_DOSBOX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DOS_SOURCE = 
RECALBOX_ROMFS_DOS_SITE = 
RECALBOX_ROMFS_DOS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DOS = dos
SYSTEM_XML_DOS = $(@D)/$(SYSTEM_NAME_DOS).xml
# System rom path
SOURCE_ROMDIR_DOS = $(RECALBOX_ROMFS_DOS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_DOSBOX),y)
define CONFIGURE_DOS
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_DOS),Dos (x86),$(SYSTEM_NAME_DOS),.pc .PC .dos .DOS,pc,pc,$(SOURCE_ROMDIR_DOS),$(@D))
endef
RECALBOX_ROMFS_DOS_CONFIGURE_CMDS += $(CONFIGURE_DOS)
endif

$(eval $(generic-package))
