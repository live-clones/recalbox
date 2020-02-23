################################################################################
#
# recalbox-romfs-gbc
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system gbc --extension '.gb .GB .gbc .GBC .zip .ZIP .7z .7Z' --fullname 'Game Boy Color' --platform gbc --theme gbc 1:libretro:gambatte:BR2_PACKAGE_LIBRETRO_GAMBATTE 2:libretro:tgbdual:BR2_PACKAGE_LIBRETRO_TGBDUAL 3:libretro:mgba:BR2_PACKAGE_LIBRETRO_MGBA 4:libretro:sameboy:BR2_PACKAGE_LIBRETRO_SAMEBOY

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GBC_SOURCE = 
RECALBOX_ROMFS_GBC_SITE = 
RECALBOX_ROMFS_GBC_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GBC = gbc
SYSTEM_XML_GBC = $(@D)/$(SYSTEM_NAME_GBC).xml
# System rom path
SOURCE_ROMDIR_GBC = $(RECALBOX_ROMFS_GBC_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_SAMEBOY),)
define CONFIGURE_MAIN_GBC_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GBC),Game Boy Color,$(SYSTEM_NAME_GBC),.gb .GB .gbc .GBC .zip .ZIP .7z .7Z,gbc,gbc)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_SAMEBOY),)
define CONFIGURE_GBC_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GBC),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MGBA),y)
define CONFIGURE_GBC_LIBRETRO_MGBA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBC),mgba,3)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE),y)
define CONFIGURE_GBC_LIBRETRO_GAMBATTE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBC),gambatte,1)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_TGBDUAL),y)
define CONFIGURE_GBC_LIBRETRO_TGBDUAL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBC),tgbdual,2)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_SAMEBOY),y)
define CONFIGURE_GBC_LIBRETRO_SAMEBOY_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBC),sameboy,4)
endef
endif

define CONFIGURE_GBC_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GBC))
endef
endif



define CONFIGURE_MAIN_GBC_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GBC),$(SOURCE_ROMDIR_GBC),$(@D))
endef
endif

define RECALBOX_ROMFS_GBC_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GBC_START)
	$(CONFIGURE_GBC_LIBRETRO_START)
	$(CONFIGURE_GBC_LIBRETRO_MGBA_DEF)
	$(CONFIGURE_GBC_LIBRETRO_GAMBATTE_DEF)
	$(CONFIGURE_GBC_LIBRETRO_TGBDUAL_DEF)
	$(CONFIGURE_GBC_LIBRETRO_SAMEBOY_DEF)
	$(CONFIGURE_GBC_LIBRETRO_END)
	$(CONFIGURE_MAIN_GBC_END)
endef

$(eval $(generic-package))
