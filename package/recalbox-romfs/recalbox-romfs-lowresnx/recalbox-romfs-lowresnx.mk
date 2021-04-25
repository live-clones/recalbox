################################################################################
#
# recalbox-romfs-lowresnx
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system lowresnx --extension '.nx .zip .7z' --fullname 'LowRes NX' --platform lowresnx --theme lowresnx 1:libretro:lowresnx:BR2_PACKAGE_LIBRETRO_LOWRES_NX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_LOWRESNX_SOURCE = 
RECALBOX_ROMFS_LOWRESNX_SITE = 
RECALBOX_ROMFS_LOWRESNX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_LOWRESNX = lowresnx
SYSTEM_XML_LOWRESNX = $(@D)/$(SYSTEM_NAME_LOWRESNX).xml
# System rom path
SOURCE_ROMDIR_LOWRESNX = $(RECALBOX_ROMFS_LOWRESNX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_LOWRES_NX),)
define CONFIGURE_MAIN_LOWRESNX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_LOWRESNX),LowRes NX,$(SYSTEM_NAME_LOWRESNX),.nx .zip .7z,lowresnx,lowresnx)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_LOWRES_NX),)
define CONFIGURE_LOWRESNX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_LOWRESNX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_LOWRES_NX),y)
define CONFIGURE_LOWRESNX_LIBRETRO_LOWRESNX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_LOWRESNX),lowresnx,1)
endef
endif

define CONFIGURE_LOWRESNX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_LOWRESNX))
endef
endif



define CONFIGURE_MAIN_LOWRESNX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_LOWRESNX),$(SOURCE_ROMDIR_LOWRESNX),$(@D))
endef
endif

define RECALBOX_ROMFS_LOWRESNX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_LOWRESNX_START)
	$(CONFIGURE_LOWRESNX_LIBRETRO_START)
	$(CONFIGURE_LOWRESNX_LIBRETRO_LOWRESNX_DEF)
	$(CONFIGURE_LOWRESNX_LIBRETRO_END)
	$(CONFIGURE_MAIN_LOWRESNX_END)
endef

$(eval $(generic-package))
