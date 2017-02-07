################################################################################
#
# recalbox-romfs-lutro
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system lutro --extension '.zip .ZIP .lua .LUA .lutro .LUTRO' --fullname 'Lutro' --platform lutro --theme lutro libretro:lutro:BR2_PACKAGE_LIBRETRO_LUTRO

# Name the 3 vars as the package requires
RECALBOX_ROMFS_LUTRO_SOURCE = 
RECALBOX_ROMFS_LUTRO_SITE = 
RECALBOX_ROMFS_LUTRO_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_LUTRO = lutro
SYSTEM_XML_LUTRO = $(@D)/$(SYSTEM_NAME_LUTRO).xml
# System rom path
SOURCE_ROMDIR_LUTRO = $(RECALBOX_ROMFS_LUTRO_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_LUTRO),)
define CONFIGURE_MAIN_LUTRO_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_LUTRO),Lutro,$(SYSTEM_NAME_LUTRO),.zip .ZIP .lua .LUA .lutro .LUTRO,lutro,lutro)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_LUTRO),)
define CONFIGURE_LUTRO_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_LUTRO),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_LUTRO),y)
define CONFIGURE_LUTRO_LIBRETRO_LUTRO_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_LUTRO),lutro)
endef
endif

define CONFIGURE_LUTRO_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_LUTRO))
endef
endif



define CONFIGURE_MAIN_LUTRO_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_LUTRO),$(SOURCE_ROMDIR_LUTRO),$(@D))
endef
endif

define RECALBOX_ROMFS_LUTRO_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_LUTRO_START)
	$(CONFIGURE_LUTRO_LIBRETRO_START)
	$(CONFIGURE_LUTRO_LIBRETRO_LUTRO_DEF)
	$(CONFIGURE_LUTRO_LIBRETRO_END)
	$(CONFIGURE_MAIN_LUTRO_END)
endef

$(eval $(generic-package))
