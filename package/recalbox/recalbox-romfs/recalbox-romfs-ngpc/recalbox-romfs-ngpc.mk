################################################################################
#
# recalbox-romfs-ngpc
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system ngpc --extension '.zip .ZIP .ngc .NGC .ngp .NGP' --fullname 'Neo-Geo Pocket Color' --platform ngpc --theme ngpc libretro:mednafen_ngp:BR2_PACKAGE_LIBRETRO_BEETLE_NGP

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NGPC_SOURCE = 
RECALBOX_ROMFS_NGPC_SITE = 
RECALBOX_ROMFS_NGPC_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NGPC = ngpc
SYSTEM_XML_NGPC = $(@D)/$(SYSTEM_NAME_NGPC).xml
# System rom path
SOURCE_ROMDIR_NGPC = $(RECALBOX_ROMFS_NGPC_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_NGP),)
define CONFIGURE_MAIN_NGPC_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NGPC),Neo-Geo Pocket Color,$(SYSTEM_NAME_NGPC),.zip .ZIP .ngc .NGC .ngp .NGP,ngpc,ngpc)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_NGP),)
define CONFIGURE_NGPC_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NGPC),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_NGP),y)
define CONFIGURE_NGPC_LIBRETRO_MEDNAFEN_NGP_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NGPC),mednafen_ngp)
endef
endif

define CONFIGURE_NGPC_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NGPC))
endef
endif



define CONFIGURE_MAIN_NGPC_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NGPC),$(SOURCE_ROMDIR_NGPC),$(@D))
endef
endif

define RECALBOX_ROMFS_NGPC_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NGPC_START)
	$(CONFIGURE_NGPC_LIBRETRO_START)
	$(CONFIGURE_NGPC_LIBRETRO_MEDNAFEN_NGP_DEF)
	$(CONFIGURE_NGPC_LIBRETRO_END)
	$(CONFIGURE_MAIN_NGPC_END)
endef

$(eval $(generic-package))
