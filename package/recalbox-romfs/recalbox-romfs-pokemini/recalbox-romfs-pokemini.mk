################################################################################
#
# recalbox-romfs-pokemini
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pokemini --extension '.min .MIN .zip .ZIP' --fullname 'Pokémon Mini' --platform pokemini --theme pokemini 1:libretro:pokemini:BR2_PACKAGE_LIBRETRO_POKEMINI

# Name the 3 vars as the package requires
RECALBOX_ROMFS_POKEMINI_SOURCE = 
RECALBOX_ROMFS_POKEMINI_SITE = 
RECALBOX_ROMFS_POKEMINI_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_POKEMINI = pokemini
SYSTEM_XML_POKEMINI = $(@D)/$(SYSTEM_NAME_POKEMINI).xml
# System rom path
SOURCE_ROMDIR_POKEMINI = $(RECALBOX_ROMFS_POKEMINI_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_POKEMINI),)
define CONFIGURE_MAIN_POKEMINI_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_POKEMINI),Pokémon Mini,$(SYSTEM_NAME_POKEMINI),.min .MIN .zip .ZIP,pokemini,pokemini)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_POKEMINI),)
define CONFIGURE_POKEMINI_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_POKEMINI),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_POKEMINI),y)
define CONFIGURE_POKEMINI_LIBRETRO_POKEMINI_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_POKEMINI),pokemini,1)
endef
endif

define CONFIGURE_POKEMINI_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_POKEMINI))
endef
endif



define CONFIGURE_MAIN_POKEMINI_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_POKEMINI),$(SOURCE_ROMDIR_POKEMINI),$(@D))
endef
endif

define RECALBOX_ROMFS_POKEMINI_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_POKEMINI_START)
	$(CONFIGURE_POKEMINI_LIBRETRO_START)
	$(CONFIGURE_POKEMINI_LIBRETRO_POKEMINI_DEF)
	$(CONFIGURE_POKEMINI_LIBRETRO_END)
	$(CONFIGURE_MAIN_POKEMINI_END)
endef

$(eval $(generic-package))