################################################################################
#
# recalbox-romfs-dos
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system dos --extension '.pc .PC .dos .DOS' --fullname 'Dos (x86)' --platform pc --theme pc 1:dosbox:dosbox:BR2_PACKAGE_DOSBOX

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


ifneq ($(BR2_PACKAGE_DOSBOX),)
define CONFIGURE_MAIN_DOS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_DOS),Dos (x86),$(SYSTEM_NAME_DOS),.pc .PC .dos .DOS,pc,pc)
endef

ifneq ($(BR2_PACKAGE_DOSBOX),)
define CONFIGURE_DOS_DOSBOX_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DOS),dosbox)
endef
ifeq ($(BR2_PACKAGE_DOSBOX),y)
define CONFIGURE_DOS_DOSBOX_DOSBOX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOS),dosbox,1)
endef
endif

define CONFIGURE_DOS_DOSBOX_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DOS))
endef
endif



define CONFIGURE_MAIN_DOS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_DOS),$(SOURCE_ROMDIR_DOS),$(@D))
endef
endif

define RECALBOX_ROMFS_DOS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DOS_START)
	$(CONFIGURE_DOS_DOSBOX_START)
	$(CONFIGURE_DOS_DOSBOX_DOSBOX_DEF)
	$(CONFIGURE_DOS_DOSBOX_END)
	$(CONFIGURE_MAIN_DOS_END)
endef

$(eval $(generic-package))
