################################################################################
#
# recalbox-romfs-dinothawr
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system dinothawr --extension '.game .zip .7z' --fullname 'Dinothawr' --platform dinothawr --theme dinothawr 1:libretro:dinothawr:BR2_PACKAGE_LIBRETRO_DINOTHAWR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DINOTHAWR_SOURCE = 
RECALBOX_ROMFS_DINOTHAWR_SITE = 
RECALBOX_ROMFS_DINOTHAWR_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DINOTHAWR = dinothawr
SYSTEM_XML_DINOTHAWR = $(@D)/$(SYSTEM_NAME_DINOTHAWR).xml
# System rom path
SOURCE_ROMDIR_DINOTHAWR = $(RECALBOX_ROMFS_DINOTHAWR_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_DINOTHAWR),)
define CONFIGURE_MAIN_DINOTHAWR_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_DINOTHAWR),Dinothawr,$(SYSTEM_NAME_DINOTHAWR),.game .zip .7z,dinothawr,dinothawr)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_DINOTHAWR),)
define CONFIGURE_DINOTHAWR_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DINOTHAWR),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_DINOTHAWR),y)
define CONFIGURE_DINOTHAWR_LIBRETRO_DINOTHAWR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DINOTHAWR),dinothawr,1)
endef
endif

define CONFIGURE_DINOTHAWR_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DINOTHAWR))
endef
endif



define CONFIGURE_MAIN_DINOTHAWR_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_DINOTHAWR),$(SOURCE_ROMDIR_DINOTHAWR),$(@D),dinothawr)
endef
endif

define RECALBOX_ROMFS_DINOTHAWR_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DINOTHAWR_START)
	$(CONFIGURE_DINOTHAWR_LIBRETRO_START)
	$(CONFIGURE_DINOTHAWR_LIBRETRO_DINOTHAWR_DEF)
	$(CONFIGURE_DINOTHAWR_LIBRETRO_END)
	$(CONFIGURE_MAIN_DINOTHAWR_END)
endef

$(eval $(generic-package))
