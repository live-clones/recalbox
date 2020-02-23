################################################################################
#
# recalbox-romfs-intellivision
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system intellivision --extension '.int .INT .rom .ROM .bin .BIN .zip .ZIP .7z .7Z' --fullname 'Mattel Intellivision' --platform intellivision --theme intellivision 1:libretro:freeintv:BR2_PACKAGE_LIBRETRO_FREEINTV

# Name the 3 vars as the package requires
RECALBOX_ROMFS_INTELLIVISION_SOURCE = 
RECALBOX_ROMFS_INTELLIVISION_SITE = 
RECALBOX_ROMFS_INTELLIVISION_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_INTELLIVISION = intellivision
SYSTEM_XML_INTELLIVISION = $(@D)/$(SYSTEM_NAME_INTELLIVISION).xml
# System rom path
SOURCE_ROMDIR_INTELLIVISION = $(RECALBOX_ROMFS_INTELLIVISION_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FREEINTV),)
define CONFIGURE_MAIN_INTELLIVISION_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_INTELLIVISION),Mattel Intellivision,$(SYSTEM_NAME_INTELLIVISION),.int .INT .rom .ROM .bin .BIN .zip .ZIP .7z .7Z,intellivision,intellivision)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FREEINTV),)
define CONFIGURE_INTELLIVISION_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_INTELLIVISION),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FREEINTV),y)
define CONFIGURE_INTELLIVISION_LIBRETRO_FREEINTV_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_INTELLIVISION),freeintv,1)
endef
endif

define CONFIGURE_INTELLIVISION_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_INTELLIVISION))
endef
endif



define CONFIGURE_MAIN_INTELLIVISION_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_INTELLIVISION),$(SOURCE_ROMDIR_INTELLIVISION),$(@D))
endef
endif

define RECALBOX_ROMFS_INTELLIVISION_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_INTELLIVISION_START)
	$(CONFIGURE_INTELLIVISION_LIBRETRO_START)
	$(CONFIGURE_INTELLIVISION_LIBRETRO_FREEINTV_DEF)
	$(CONFIGURE_INTELLIVISION_LIBRETRO_END)
	$(CONFIGURE_MAIN_INTELLIVISION_END)
endef

$(eval $(generic-package))
