################################################################################
#
# recalbox-romfs-channelf
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system channelf --extension '.bin .BIN .rom .ROM .zip .ZIP .7z .7Z' --fullname 'Fairchild Channel F' --platform channelf --theme channelf 1:libretro:freechaf:BR2_PACKAGE_LIBRETRO_FREECHAF

# Name the 3 vars as the package requires
RECALBOX_ROMFS_CHANNELF_SOURCE = 
RECALBOX_ROMFS_CHANNELF_SITE = 
RECALBOX_ROMFS_CHANNELF_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_CHANNELF = channelf
SYSTEM_XML_CHANNELF = $(@D)/$(SYSTEM_NAME_CHANNELF).xml
# System rom path
SOURCE_ROMDIR_CHANNELF = $(RECALBOX_ROMFS_CHANNELF_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FREECHAF),)
define CONFIGURE_MAIN_CHANNELF_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_CHANNELF),Fairchild Channel F,$(SYSTEM_NAME_CHANNELF),.bin .BIN .rom .ROM .zip .ZIP .7z .7Z,channelf,channelf)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FREECHAF),)
define CONFIGURE_CHANNELF_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_CHANNELF),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FREECHAF),y)
define CONFIGURE_CHANNELF_LIBRETRO_FREECHAF_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_CHANNELF),freechaf,1)
endef
endif

define CONFIGURE_CHANNELF_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_CHANNELF))
endef
endif



define CONFIGURE_MAIN_CHANNELF_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_CHANNELF),$(SOURCE_ROMDIR_CHANNELF),$(@D))
endef
endif

define RECALBOX_ROMFS_CHANNELF_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_CHANNELF_START)
	$(CONFIGURE_CHANNELF_LIBRETRO_START)
	$(CONFIGURE_CHANNELF_LIBRETRO_FREECHAF_DEF)
	$(CONFIGURE_CHANNELF_LIBRETRO_END)
	$(CONFIGURE_MAIN_CHANNELF_END)
endef

$(eval $(generic-package))
