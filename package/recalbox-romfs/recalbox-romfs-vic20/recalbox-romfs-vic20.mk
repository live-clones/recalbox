################################################################################
#
# recalbox-romfs-vic20
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system vic20 --extension '.20 .40 .60 .a0 .b0 .bin .zip .7z' --fullname 'Commodore VIC-20' --platform vic20 --theme vic20 1:libretro:vice_xvic:BR2_PACKAGE_LIBRETRO_VICE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_VIC20_SOURCE = 
RECALBOX_ROMFS_VIC20_SITE = 
RECALBOX_ROMFS_VIC20_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_VIC20 = vic20
SYSTEM_XML_VIC20 = $(@D)/$(SYSTEM_NAME_VIC20).xml
# System rom path
SOURCE_ROMDIR_VIC20 = $(RECALBOX_ROMFS_VIC20_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VICE),)
define CONFIGURE_MAIN_VIC20_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_VIC20),Commodore VIC-20,$(SYSTEM_NAME_VIC20),.20 .40 .60 .a0 .b0 .bin .zip .7z,vic20,vic20)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VICE),)
define CONFIGURE_VIC20_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_VIC20),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_VIC20_LIBRETRO_VICE_XVIC_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_VIC20),vice_xvic,1)
endef
endif

define CONFIGURE_VIC20_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_VIC20))
endef
endif



define CONFIGURE_MAIN_VIC20_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_VIC20),$(SOURCE_ROMDIR_VIC20),$(@D))
endef
endif

define RECALBOX_ROMFS_VIC20_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_VIC20_START)
	$(CONFIGURE_VIC20_LIBRETRO_START)
	$(CONFIGURE_VIC20_LIBRETRO_VICE_XVIC_DEF)
	$(CONFIGURE_VIC20_LIBRETRO_END)
	$(CONFIGURE_MAIN_VIC20_END)
endef

$(eval $(generic-package))
