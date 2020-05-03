################################################################################
#
# recalbox-romfs-quake3
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system quake3 --extension '.pak3 .zip .7z' --fullname 'Quake 3' --platform quake3 --theme quake 1:libretro:vitaquake3:BR2_PACKAGE_LIBRETRO_VITAQUAKE3

# Name the 3 vars as the package requires
RECALBOX_ROMFS_QUAKE3_SOURCE = 
RECALBOX_ROMFS_QUAKE3_SITE = 
RECALBOX_ROMFS_QUAKE3_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_QUAKE3 = quake3
SYSTEM_XML_QUAKE3 = $(@D)/$(SYSTEM_NAME_QUAKE3).xml
# System rom path
SOURCE_ROMDIR_QUAKE3 = $(RECALBOX_ROMFS_QUAKE3_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE3),)
define CONFIGURE_MAIN_QUAKE3_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_QUAKE3),Quake 3,$(SYSTEM_NAME_QUAKE3),.pak3 .zip .7z,quake3,quake)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE3),)
define CONFIGURE_QUAKE3_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_QUAKE3),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE3),y)
define CONFIGURE_QUAKE3_LIBRETRO_VITAQUAKE3_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_QUAKE3),vitaquake3,1)
endef
endif

define CONFIGURE_QUAKE3_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_QUAKE3))
endef
endif



define CONFIGURE_MAIN_QUAKE3_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_QUAKE3),$(SOURCE_ROMDIR_QUAKE3),$(@D),quake3)
endef
endif

define RECALBOX_ROMFS_QUAKE3_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_QUAKE3_START)
	$(CONFIGURE_QUAKE3_LIBRETRO_START)
	$(CONFIGURE_QUAKE3_LIBRETRO_VITAQUAKE3_DEF)
	$(CONFIGURE_QUAKE3_LIBRETRO_END)
	$(CONFIGURE_MAIN_QUAKE3_END)
endef

$(eval $(generic-package))
