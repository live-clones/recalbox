################################################################################
#
# recalbox-romfs-scv
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system scv --extension '.bin .cart .zip .7z' --fullname 'Super Cassette Vision' --platform scv --theme scv 1:libretro:emuscv:BR2_PACKAGE_LIBRETRO_EMUSCV

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SCV_SOURCE = 
RECALBOX_ROMFS_SCV_SITE = 
RECALBOX_ROMFS_SCV_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SCV = scv
SYSTEM_XML_SCV = $(@D)/$(SYSTEM_NAME_SCV).xml
# System rom path
SOURCE_ROMDIR_SCV = $(RECALBOX_ROMFS_SCV_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_EMUSCV),)
define CONFIGURE_MAIN_SCV_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SCV),Super Cassette Vision,$(SYSTEM_NAME_SCV),.bin .cart .zip .7z,scv,scv)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_EMUSCV),)
define CONFIGURE_SCV_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SCV),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_EMUSCV),y)
define CONFIGURE_SCV_LIBRETRO_EMUSCV_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SCV),emuscv,1)
endef
endif

define CONFIGURE_SCV_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SCV))
endef
endif



define CONFIGURE_MAIN_SCV_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SCV),$(SOURCE_ROMDIR_SCV),$(@D))
endef
endif

define RECALBOX_ROMFS_SCV_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SCV_START)
	$(CONFIGURE_SCV_LIBRETRO_START)
	$(CONFIGURE_SCV_LIBRETRO_EMUSCV_DEF)
	$(CONFIGURE_SCV_LIBRETRO_END)
	$(CONFIGURE_MAIN_SCV_END)
endef

$(eval $(generic-package))
