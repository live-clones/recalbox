################################################################################
#
# recalbox-romfs-multivision
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system multivision --extension '.mv .MV .bin .BIN .zip .ZIP .7z .7Z' --fullname 'Othello Multivision' --platform multivision --theme multivision 1:libretro:gearsystem:BR2_PACKAGE_LIBRETRO_GEARSYSTEM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MULTIVISION_SOURCE = 
RECALBOX_ROMFS_MULTIVISION_SITE = 
RECALBOX_ROMFS_MULTIVISION_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MULTIVISION = multivision
SYSTEM_XML_MULTIVISION = $(@D)/$(SYSTEM_NAME_MULTIVISION).xml
# System rom path
SOURCE_ROMDIR_MULTIVISION = $(RECALBOX_ROMFS_MULTIVISION_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GEARSYSTEM),)
define CONFIGURE_MAIN_MULTIVISION_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MULTIVISION),Othello Multivision,$(SYSTEM_NAME_MULTIVISION),.mv .MV .bin .BIN .zip .ZIP .7z .7Z,multivision,multivision)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GEARSYSTEM),)
define CONFIGURE_MULTIVISION_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MULTIVISION),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GEARSYSTEM),y)
define CONFIGURE_MULTIVISION_LIBRETRO_GEARSYSTEM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MULTIVISION),gearsystem,1)
endef
endif

define CONFIGURE_MULTIVISION_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MULTIVISION))
endef
endif



define CONFIGURE_MAIN_MULTIVISION_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MULTIVISION),$(SOURCE_ROMDIR_MULTIVISION),$(@D))
endef
endif

define RECALBOX_ROMFS_MULTIVISION_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MULTIVISION_START)
	$(CONFIGURE_MULTIVISION_LIBRETRO_START)
	$(CONFIGURE_MULTIVISION_LIBRETRO_GEARSYSTEM_DEF)
	$(CONFIGURE_MULTIVISION_LIBRETRO_END)
	$(CONFIGURE_MAIN_MULTIVISION_END)
endef

$(eval $(generic-package))
