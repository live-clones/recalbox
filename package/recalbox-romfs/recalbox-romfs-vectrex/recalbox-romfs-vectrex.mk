################################################################################
#
# recalbox-romfs-vectrex
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system vectrex --extension '.zip .ZIP .vec .VEC' --fullname 'Vectrex' --platform vectrex --theme vectrex libretro:vecx:BR2_PACKAGE_LIBRETRO_VECX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_VECTREX_SOURCE = 
RECALBOX_ROMFS_VECTREX_SITE = 
RECALBOX_ROMFS_VECTREX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_VECTREX = vectrex
SYSTEM_XML_VECTREX = $(@D)/$(SYSTEM_NAME_VECTREX).xml
# System rom path
SOURCE_ROMDIR_VECTREX = $(RECALBOX_ROMFS_VECTREX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VECX),)
define CONFIGURE_MAIN_VECTREX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_VECTREX),Vectrex,$(SYSTEM_NAME_VECTREX),.zip .ZIP .vec .VEC,vectrex,vectrex)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VECX),)
define CONFIGURE_VECTREX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_VECTREX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VECX),y)
define CONFIGURE_VECTREX_LIBRETRO_VECX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_VECTREX),vecx)
endef
endif

define CONFIGURE_VECTREX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_VECTREX))
endef
endif



define CONFIGURE_MAIN_VECTREX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_VECTREX),$(SOURCE_ROMDIR_VECTREX),$(@D))
endef
endif

define RECALBOX_ROMFS_VECTREX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_VECTREX_START)
	$(CONFIGURE_VECTREX_LIBRETRO_START)
	$(CONFIGURE_VECTREX_LIBRETRO_VECX_DEF)
	$(CONFIGURE_VECTREX_LIBRETRO_END)
	$(CONFIGURE_MAIN_VECTREX_END)
endef

$(eval $(generic-package))
