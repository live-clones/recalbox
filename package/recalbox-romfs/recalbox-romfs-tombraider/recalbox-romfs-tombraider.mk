################################################################################
#
# recalbox-romfs-tombraider
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system tombraider --extension '.phd .psx .tr2 ' --fullname 'Tombraider' --platform tombraider --theme tombraider 1:libretro:openlara:BR2_PACKAGE_LIBRETRO_OPENLARA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_TOMBRAIDER_SOURCE = 
RECALBOX_ROMFS_TOMBRAIDER_SITE = 
RECALBOX_ROMFS_TOMBRAIDER_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_TOMBRAIDER = tombraider
SYSTEM_XML_TOMBRAIDER = $(@D)/$(SYSTEM_NAME_TOMBRAIDER).xml
# System rom path
SOURCE_ROMDIR_TOMBRAIDER = $(RECALBOX_ROMFS_TOMBRAIDER_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),)
define CONFIGURE_MAIN_TOMBRAIDER_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_TOMBRAIDER),Tombraider,$(SYSTEM_NAME_TOMBRAIDER),.phd .psx .tr2 ,tombraider,tombraider)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),)
define CONFIGURE_TOMBRAIDER_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_TOMBRAIDER),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),y)
define CONFIGURE_TOMBRAIDER_LIBRETRO_OPENLARA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_TOMBRAIDER),openlara,1)
endef
endif

define CONFIGURE_TOMBRAIDER_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_TOMBRAIDER))
endef
endif



define CONFIGURE_MAIN_TOMBRAIDER_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_TOMBRAIDER),$(SOURCE_ROMDIR_TOMBRAIDER),$(@D),tombraider)
endef
endif

define RECALBOX_ROMFS_TOMBRAIDER_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_TOMBRAIDER_START)
	$(CONFIGURE_TOMBRAIDER_LIBRETRO_START)
	$(CONFIGURE_TOMBRAIDER_LIBRETRO_OPENLARA_DEF)
	$(CONFIGURE_TOMBRAIDER_LIBRETRO_END)
	$(CONFIGURE_MAIN_TOMBRAIDER_END)
endef

$(eval $(generic-package))
