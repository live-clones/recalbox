################################################################################
#
# recalbox-romfs-bk
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system bk --extension '.bin .zip .7z' --fullname 'Elektronika BK' --platform bk --theme bk 1:libretro:bk:BR2_PACKAGE_LIBRETRO_BK_EMULATOR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_BK_SOURCE = 
RECALBOX_ROMFS_BK_SITE = 
RECALBOX_ROMFS_BK_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_BK = bk
SYSTEM_XML_BK = $(@D)/$(SYSTEM_NAME_BK).xml
# System rom path
SOURCE_ROMDIR_BK = $(RECALBOX_ROMFS_BK_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BK_EMULATOR),)
define CONFIGURE_MAIN_BK_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_BK),Elektronika BK,$(SYSTEM_NAME_BK),.bin .zip .7z,bk,bk)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BK_EMULATOR),)
define CONFIGURE_BK_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_BK),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BK_EMULATOR),y)
define CONFIGURE_BK_LIBRETRO_BK_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_BK),bk,1)
endef
endif

define CONFIGURE_BK_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_BK))
endef
endif



define CONFIGURE_MAIN_BK_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_BK),$(SOURCE_ROMDIR_BK),$(@D))
endef
endif

define RECALBOX_ROMFS_BK_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_BK_START)
	$(CONFIGURE_BK_LIBRETRO_START)
	$(CONFIGURE_BK_LIBRETRO_BK_DEF)
	$(CONFIGURE_BK_LIBRETRO_END)
	$(CONFIGURE_MAIN_BK_END)
endef

$(eval $(generic-package))
