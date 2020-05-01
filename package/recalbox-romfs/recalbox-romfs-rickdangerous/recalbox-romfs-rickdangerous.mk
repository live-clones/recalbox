################################################################################
#
# recalbox-romfs-rickdangerous
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system rickdangerous --extension '.zip .7z' --fullname 'Rick Dangerous' --platform rickdangerous --theme xrick 1:libretro:xrick:BR2_PACKAGE_LIBRETRO_XRICK

# Name the 3 vars as the package requires
RECALBOX_ROMFS_RICKDANGEROUS_SOURCE = 
RECALBOX_ROMFS_RICKDANGEROUS_SITE = 
RECALBOX_ROMFS_RICKDANGEROUS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_RICKDANGEROUS = rickdangerous
SYSTEM_XML_RICKDANGEROUS = $(@D)/$(SYSTEM_NAME_RICKDANGEROUS).xml
# System rom path
SOURCE_ROMDIR_RICKDANGEROUS = $(RECALBOX_ROMFS_RICKDANGEROUS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_XRICK),)
define CONFIGURE_MAIN_RICKDANGEROUS_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_RICKDANGEROUS),Rick Dangerous,$(SYSTEM_NAME_RICKDANGEROUS),.zip .7z,rickdangerous,xrick)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_XRICK),)
define CONFIGURE_RICKDANGEROUS_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_RICKDANGEROUS),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_XRICK),y)
define CONFIGURE_RICKDANGEROUS_LIBRETRO_XRICK_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_RICKDANGEROUS),xrick,1)
endef
endif

define CONFIGURE_RICKDANGEROUS_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_RICKDANGEROUS))
endef
endif



define CONFIGURE_MAIN_RICKDANGEROUS_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_RICKDANGEROUS),$(SOURCE_ROMDIR_RICKDANGEROUS),$(@D),rickdangerous)
endef
endif

define RECALBOX_ROMFS_RICKDANGEROUS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_RICKDANGEROUS_START)
	$(CONFIGURE_RICKDANGEROUS_LIBRETRO_START)
	$(CONFIGURE_RICKDANGEROUS_LIBRETRO_XRICK_DEF)
	$(CONFIGURE_RICKDANGEROUS_LIBRETRO_END)
	$(CONFIGURE_MAIN_RICKDANGEROUS_END)
endef

$(eval $(generic-package))
