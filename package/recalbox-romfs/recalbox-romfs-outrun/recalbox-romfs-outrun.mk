################################################################################
#
# recalbox-romfs-outrun
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system outrun --extension '.88' --fullname 'Out Run' --platform outrun --theme outrun 1:libretro:cannonball:BR2_PACKAGE_LIBRETRO_CANNONBALL

# Name the 3 vars as the package requires
RECALBOX_ROMFS_OUTRUN_SOURCE = 
RECALBOX_ROMFS_OUTRUN_SITE = 
RECALBOX_ROMFS_OUTRUN_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_OUTRUN = outrun
SYSTEM_XML_OUTRUN = $(@D)/$(SYSTEM_NAME_OUTRUN).xml
# System rom path
SOURCE_ROMDIR_OUTRUN = $(RECALBOX_ROMFS_OUTRUN_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_CANNONBALL),)
define CONFIGURE_MAIN_OUTRUN_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_OUTRUN),Out Run,$(SYSTEM_NAME_OUTRUN),.88,outrun,outrun)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_CANNONBALL),)
define CONFIGURE_OUTRUN_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_OUTRUN),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_CANNONBALL),y)
define CONFIGURE_OUTRUN_LIBRETRO_CANNONBALL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_OUTRUN),cannonball,1)
endef
endif

define CONFIGURE_OUTRUN_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_OUTRUN))
endef
endif



define CONFIGURE_MAIN_OUTRUN_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_OUTRUN),$(SOURCE_ROMDIR_OUTRUN),$(@D),outrun)
endef
endif

define RECALBOX_ROMFS_OUTRUN_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_OUTRUN_START)
	$(CONFIGURE_OUTRUN_LIBRETRO_START)
	$(CONFIGURE_OUTRUN_LIBRETRO_CANNONBALL_DEF)
	$(CONFIGURE_OUTRUN_LIBRETRO_END)
	$(CONFIGURE_MAIN_OUTRUN_END)
endef

$(eval $(generic-package))

