################################################################################
#
# recalbox-romfs-openlara
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system openlara --extension '.phd .psx .tr2 ' --fullname 'OpenLara' --platform openlara --theme openlara 1:libretro:openlara:BR2_PACKAGE_LIBRETRO_OPENLARA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_OPENLARA_SOURCE = 
RECALBOX_ROMFS_OPENLARA_SITE = 
RECALBOX_ROMFS_OPENLARA_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_OPENLARA = openlara
SYSTEM_XML_OPENLARA = $(@D)/$(SYSTEM_NAME_OPENLARA).xml
# System rom path
SOURCE_ROMDIR_OPENLARA = $(RECALBOX_ROMFS_OPENLARA_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),)
define CONFIGURE_MAIN_OPENLARA_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_OPENLARA),OpenLara,$(SYSTEM_NAME_OPENLARA),.phd .psx .tr2 ,openlara,openlara)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),)
define CONFIGURE_OPENLARA_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_OPENLARA),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_OPENLARA),y)
define CONFIGURE_OPENLARA_LIBRETRO_OPENLARA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_OPENLARA),openlara,1)
endef
endif

define CONFIGURE_OPENLARA_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_OPENLARA))
endef
endif



define CONFIGURE_MAIN_OPENLARA_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_OPENLARA),$(SOURCE_ROMDIR_OPENLARA),$(@D),openlara)
endef
endif

define RECALBOX_ROMFS_OPENLARA_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_OPENLARA_START)
	$(CONFIGURE_OPENLARA_LIBRETRO_START)
	$(CONFIGURE_OPENLARA_LIBRETRO_OPENLARA_DEF)
	$(CONFIGURE_OPENLARA_LIBRETRO_END)
	$(CONFIGURE_MAIN_OPENLARA_END)
endef

$(eval $(generic-package))
