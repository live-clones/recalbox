################################################################################
#
# recalbox-romfs-mrboom
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system mrboom --extension '.game' --fullname 'Mr. Boom' --platform mrboom --theme mrboom 1:libretro:mrboom:BR2_PACKAGE_LIBRETRO_MRBOOM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MRBOOM_SOURCE = 
RECALBOX_ROMFS_MRBOOM_SITE = 
RECALBOX_ROMFS_MRBOOM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MRBOOM = mrboom
SYSTEM_XML_MRBOOM = $(@D)/$(SYSTEM_NAME_MRBOOM).xml
# System rom path
SOURCE_ROMDIR_MRBOOM = $(RECALBOX_ROMFS_MRBOOM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),)
define CONFIGURE_MAIN_MRBOOM_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_MRBOOM),Mr. Boom,$(SYSTEM_NAME_MRBOOM),.game,mrboom,mrboom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),)
define CONFIGURE_MRBOOM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MRBOOM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),y)
define CONFIGURE_MRBOOM_LIBRETRO_MRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MRBOOM),mrboom,1)
endef
endif

define CONFIGURE_MRBOOM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MRBOOM))
endef
endif



define CONFIGURE_MAIN_MRBOOM_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_MRBOOM),$(SOURCE_ROMDIR_MRBOOM),$(@D),mrboom)
endef
endif

define RECALBOX_ROMFS_MRBOOM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MRBOOM_START)
	$(CONFIGURE_MRBOOM_LIBRETRO_START)
	$(CONFIGURE_MRBOOM_LIBRETRO_MRBOOM_DEF)
	$(CONFIGURE_MRBOOM_LIBRETRO_END)
	$(CONFIGURE_MAIN_MRBOOM_END)
endef

$(eval $(generic-package))
