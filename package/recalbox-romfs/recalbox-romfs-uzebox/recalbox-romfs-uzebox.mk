################################################################################
#
# recalbox-romfs-uzebox
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system uzebox --extension '.uze .UZE .zip .ZIP .7z .7Z' --fullname 'Uzebox' --platform uzebox --theme uzebox 1:libretro:uzem:BR2_PACKAGE_LIBRETRO_UZEM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_UZEBOX_SOURCE = 
RECALBOX_ROMFS_UZEBOX_SITE = 
RECALBOX_ROMFS_UZEBOX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_UZEBOX = uzebox
SYSTEM_XML_UZEBOX = $(@D)/$(SYSTEM_NAME_UZEBOX).xml
# System rom path
SOURCE_ROMDIR_UZEBOX = $(RECALBOX_ROMFS_UZEBOX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_UZEM),)
define CONFIGURE_MAIN_UZEBOX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_UZEBOX),Uzebox,$(SYSTEM_NAME_UZEBOX),.uze .UZE .zip .ZIP .7z .7Z,uzebox,uzebox)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_UZEM),)
define CONFIGURE_UZEBOX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_UZEBOX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_UZEM),y)
define CONFIGURE_UZEBOX_LIBRETRO_UZEM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_UZEBOX),uzem,1)
endef
endif

define CONFIGURE_UZEBOX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_UZEBOX))
endef
endif



define CONFIGURE_MAIN_UZEBOX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_UZEBOX),$(SOURCE_ROMDIR_UZEBOX),$(@D))
endef
endif

define RECALBOX_ROMFS_UZEBOX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_UZEBOX_START)
	$(CONFIGURE_UZEBOX_LIBRETRO_START)
	$(CONFIGURE_UZEBOX_LIBRETRO_UZEM_DEF)
	$(CONFIGURE_UZEBOX_LIBRETRO_END)
	$(CONFIGURE_MAIN_UZEBOX_END)
endef

$(eval $(generic-package))
