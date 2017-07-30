################################################################################
#
# recalbox-romfs-amstradcpc
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system amstradcpc --extension '.dsk .DSK .zip .ZIP' --fullname 'AmstradCPC' --platform amstradcpc --theme amstradcpc libretro:cap32:BR2_PACKAGE_LIBRETRO_CAP32

# Name the 3 vars as the package requires
RECALBOX_ROMFS_AMSTRADCPC_SOURCE = 
RECALBOX_ROMFS_AMSTRADCPC_SITE = 
RECALBOX_ROMFS_AMSTRADCPC_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_AMSTRADCPC = amstradcpc
SYSTEM_XML_AMSTRADCPC = $(@D)/$(SYSTEM_NAME_AMSTRADCPC).xml
# System rom path
SOURCE_ROMDIR_AMSTRADCPC = $(RECALBOX_ROMFS_AMSTRADCPC_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_CAP32),)
define CONFIGURE_MAIN_AMSTRADCPC_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_AMSTRADCPC),AmstradCPC,$(SYSTEM_NAME_AMSTRADCPC),.dsk .DSK .zip .ZIP,amstradcpc,amstradcpc)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_CAP32),)
define CONFIGURE_AMSTRADCPC_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_AMSTRADCPC),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_CAP32),y)
define CONFIGURE_AMSTRADCPC_LIBRETRO_CAP32_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_AMSTRADCPC),cap32)
endef
endif

define CONFIGURE_AMSTRADCPC_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_AMSTRADCPC))
endef
endif



define CONFIGURE_MAIN_AMSTRADCPC_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_AMSTRADCPC),$(SOURCE_ROMDIR_AMSTRADCPC),$(@D))
endef
endif

define RECALBOX_ROMFS_AMSTRADCPC_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_AMSTRADCPC_START)
	$(CONFIGURE_AMSTRADCPC_LIBRETRO_START)
	$(CONFIGURE_AMSTRADCPC_LIBRETRO_CAP32_DEF)
	$(CONFIGURE_AMSTRADCPC_LIBRETRO_END)
	$(CONFIGURE_MAIN_AMSTRADCPC_END)
endef

$(eval $(generic-package))
