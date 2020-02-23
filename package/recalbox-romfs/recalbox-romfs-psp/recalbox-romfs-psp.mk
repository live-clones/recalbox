################################################################################
#
# recalbox-romfs-psp
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system psp --extension '.iso .ISO .cso .CSO .pbp .PBP' --fullname 'Sony Playstation Portable' --platform psp --theme psp 1:ppsspp:ppsspp:BR2_PACKAGE_PPSSPP

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


ifneq ($(BR2_PACKAGE_PPSSPP),)
define CONFIGURE_MAIN_PSP_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PSP),Sony Playstation Portable,$(SYSTEM_NAME_PSP),.iso .ISO .cso .CSO .pbp .PBP,psp,psp)
endef

ifneq ($(BR2_PACKAGE_PPSSPP),)
define CONFIGURE_PSP_PPSSPP_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PSP),ppsspp)
endef
ifeq ($(BR2_PACKAGE_PPSSPP),y)
define CONFIGURE_PSP_PPSSPP_PPSSPP_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PSP),ppsspp,1)
endef
endif

define CONFIGURE_PSP_PPSSPP_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PSP))
endef
endif



define CONFIGURE_MAIN_PSP_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PSP),$(SOURCE_ROMDIR_PSP),$(@D))
endef
endif

define RECALBOX_ROMFS_PSP_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PSP_START)
	$(CONFIGURE_PSP_PPSSPP_START)
	$(CONFIGURE_PSP_PPSSPP_PPSSPP_DEF)
	$(CONFIGURE_PSP_PPSSPP_END)
	$(CONFIGURE_MAIN_PSP_END)
endef

$(eval $(generic-package))
