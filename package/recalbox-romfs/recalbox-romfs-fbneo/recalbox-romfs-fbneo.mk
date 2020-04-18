################################################################################
#
# recalbox-romfs-fbneo
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system fbneo --extension '.zip .ZIP .fba .FBA .7z .7Z' --fullname 'FinalBurn Neo' --platform arcade --theme fbneo 1:libretro:fbneo:BR2_PACKAGE_LIBRETRO_FBNEO

# Name the 3 vars as the package requires
RECALBOX_ROMFS_FBNEO_SOURCE = 
RECALBOX_ROMFS_FBNEO_SITE = 
RECALBOX_ROMFS_FBNEO_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_FBNEO = fbneo
SYSTEM_XML_FBNEO = $(@D)/$(SYSTEM_NAME_FBNEO).xml
# System rom path
SOURCE_ROMDIR_FBNEO = $(RECALBOX_ROMFS_FBNEO_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FBNEO),)
define CONFIGURE_MAIN_FBNEO_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_FBNEO),FinalBurn Neo,$(SYSTEM_NAME_FBNEO),.zip .ZIP .fba .FBA .7z .7Z,arcade,fbneo)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FBNEO),)
define CONFIGURE_FBNEO_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_FBNEO),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FBNEO),y)
define CONFIGURE_FBNEO_LIBRETRO_FBNEO_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_FBNEO),fbneo,1)
endef
endif

define CONFIGURE_FBNEO_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_FBNEO))
endef
endif



define CONFIGURE_MAIN_FBNEO_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_FBNEO),$(SOURCE_ROMDIR_FBNEO),$(@D))
endef
endif

define RECALBOX_ROMFS_FBNEO_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_FBNEO_START)
	$(CONFIGURE_FBNEO_LIBRETRO_START)
	$(CONFIGURE_FBNEO_LIBRETRO_FBNEO_DEF)
	$(CONFIGURE_FBNEO_LIBRETRO_END)
	$(CONFIGURE_MAIN_FBNEO_END)
endef

$(eval $(generic-package))
