################################################################################
#
# recalbox-romfs-psp
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system psp --extension '.iso .ISO .cso .CSO' --fullname 'Sony Playstation Portable' --platform psp --theme psp BR2_PACKAGE_PPSSPP

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PSP_SOURCE = 
RECALBOX_ROMFS_PSP_SITE = 
RECALBOX_ROMFS_PSP_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PSP = psp
SYSTEM_XML_PSP = $(@D)/$(SYSTEM_NAME_PSP).xml
# System rom path
SOURCE_ROMDIR_PSP = $(RECALBOX_ROMFS_PSP_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_PPSSPP),y)
define CONFIGURE_PSP
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_PSP),Sony Playstation Portable,$(SYSTEM_NAME_PSP),.iso .ISO .cso .CSO .pbp .PBP,psp,psp,$(SOURCE_ROMDIR_PSP),$(@D))
endef
RECALBOX_ROMFS_PSP_CONFIGURE_CMDS += $(CONFIGURE_PSP)
endif

$(eval $(generic-package))
