################################################################################
#
# recalbox-romfs-videopacplus
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system videopacplus --extension '.bin .zip .7z' --fullname 'Videopac+ G7400' --platform videopacplus --theme videopacplus 1:libretro:o2em:BR2_PACKAGE_LIBRETRO_O2EM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_VIDEOPACPLUS_SOURCE = 
RECALBOX_ROMFS_VIDEOPACPLUS_SITE = 
RECALBOX_ROMFS_VIDEOPACPLUS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_VIDEOPACPLUS = videopacplus
SYSTEM_XML_VIDEOPACPLUS = $(@D)/$(SYSTEM_NAME_VIDEOPACPLUS).xml
# System rom path
SOURCE_ROMDIR_VIDEOPACPLUS = $(RECALBOX_ROMFS_VIDEOPACPLUS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_O2EM),)
define CONFIGURE_MAIN_VIDEOPACPLUS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_VIDEOPACPLUS),Videopac+ G7400,$(SYSTEM_NAME_VIDEOPACPLUS),.bin .zip .7z,videopacplus,videopacplus)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_O2EM),)
define CONFIGURE_VIDEOPACPLUS_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_VIDEOPACPLUS),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_O2EM),y)
define CONFIGURE_VIDEOPACPLUS_LIBRETRO_O2EM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_VIDEOPACPLUS),o2em,1)
endef
endif

define CONFIGURE_VIDEOPACPLUS_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_VIDEOPACPLUS))
endef
endif



define CONFIGURE_MAIN_VIDEOPACPLUS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_VIDEOPACPLUS),$(SOURCE_ROMDIR_VIDEOPACPLUS),$(@D))
endef
endif

define RECALBOX_ROMFS_VIDEOPACPLUS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_VIDEOPACPLUS_START)
	$(CONFIGURE_VIDEOPACPLUS_LIBRETRO_START)
	$(CONFIGURE_VIDEOPACPLUS_LIBRETRO_O2EM_DEF)
	$(CONFIGURE_VIDEOPACPLUS_LIBRETRO_END)
	$(CONFIGURE_MAIN_VIDEOPACPLUS_END)
endef

$(eval $(generic-package))
