################################################################################
#
# recalbox-romfs-wolfenstein3d
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system wolfenstein3d --extension '.pk3 .exe .zip .7z' --fullname 'Wolfenstein 3D' --platform wolfenstein3d --theme wolfenstein3d 1:libretro:ecwolf:BR2_PACKAGE_LIBRETRO_ECWOLF

# Name the 3 vars as the package requires
RECALBOX_ROMFS_WOLFENSTEIN3D_SOURCE = 
RECALBOX_ROMFS_WOLFENSTEIN3D_SITE = 
RECALBOX_ROMFS_WOLFENSTEIN3D_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_WOLFENSTEIN3D = wolfenstein3d
SYSTEM_XML_WOLFENSTEIN3D = $(@D)/$(SYSTEM_NAME_WOLFENSTEIN3D).xml
# System rom path
SOURCE_ROMDIR_WOLFENSTEIN3D = $(RECALBOX_ROMFS_WOLFENSTEIN3D_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_ECWOLF),)
define CONFIGURE_MAIN_WOLFENSTEIN3D_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_WOLFENSTEIN3D),Wolfenstein 3D,$(SYSTEM_NAME_WOLFENSTEIN3D),.pk3 .exe .zip .7z,wolfenstein3d,wolfenstein3d)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_ECWOLF),)
define CONFIGURE_WOLFENSTEIN3D_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_WOLFENSTEIN3D),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_ECWOLF),y)
define CONFIGURE_WOLFENSTEIN3D_LIBRETRO_ECWOLF_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_WOLFENSTEIN3D),ecwolf,1)
endef
endif

define CONFIGURE_WOLFENSTEIN3D_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_WOLFENSTEIN3D))
endef
endif



define CONFIGURE_MAIN_WOLFENSTEIN3D_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_WOLFENSTEIN3D),$(SOURCE_ROMDIR_WOLFENSTEIN3D),$(@D),wolfenstein3d)
endef
endif

define RECALBOX_ROMFS_WOLFENSTEIN3D_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_WOLFENSTEIN3D_START)
	$(CONFIGURE_WOLFENSTEIN3D_LIBRETRO_START)
	$(CONFIGURE_WOLFENSTEIN3D_LIBRETRO_ECWOLF_DEF)
	$(CONFIGURE_WOLFENSTEIN3D_LIBRETRO_END)
	$(CONFIGURE_MAIN_WOLFENSTEIN3D_END)
endef

$(eval $(generic-package))
