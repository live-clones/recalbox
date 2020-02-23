################################################################################
#
# recalbox-romfs-apple2
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system apple2 --extension '.nib .NIB .do .DO .po .PO .dsk .DSK' --fullname 'Apple II' --platform apple2 --theme apple2 1:linapple:linapple:BR2_PACKAGE_LINAPPLE_PIE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_APPLE2_SOURCE = 
RECALBOX_ROMFS_APPLE2_SITE = 
RECALBOX_ROMFS_APPLE2_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_APPLE2 = apple2
SYSTEM_XML_APPLE2 = $(@D)/$(SYSTEM_NAME_APPLE2).xml
# System rom path
SOURCE_ROMDIR_APPLE2 = $(RECALBOX_ROMFS_APPLE2_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LINAPPLE_PIE),)
define CONFIGURE_MAIN_APPLE2_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_APPLE2),Apple II,$(SYSTEM_NAME_APPLE2),.nib .NIB .do .DO .po .PO .dsk .DSK,apple2,apple2)
endef

ifneq ($(BR2_PACKAGE_LINAPPLE_PIE),)
define CONFIGURE_APPLE2_LINAPPLE_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_APPLE2),linapple)
endef
ifeq ($(BR2_PACKAGE_LINAPPLE_PIE),y)
define CONFIGURE_APPLE2_LINAPPLE_LINAPPLE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_APPLE2),linapple,1)
endef
endif

define CONFIGURE_APPLE2_LINAPPLE_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_APPLE2))
endef
endif



define CONFIGURE_MAIN_APPLE2_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_APPLE2),$(SOURCE_ROMDIR_APPLE2),$(@D))
endef
endif

define RECALBOX_ROMFS_APPLE2_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_APPLE2_START)
	$(CONFIGURE_APPLE2_LINAPPLE_START)
	$(CONFIGURE_APPLE2_LINAPPLE_LINAPPLE_DEF)
	$(CONFIGURE_APPLE2_LINAPPLE_END)
	$(CONFIGURE_MAIN_APPLE2_END)
endef

$(eval $(generic-package))
