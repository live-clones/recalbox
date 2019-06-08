################################################################################
#
# recalbox-romfs-uzebox
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system uzebox --extension '.uze .UZE .zip .ZIP .7z .7Z' --fullname 'Uzebox' --platform uzebox --theme uzebox libretro:uzem:BR2_PACKAGE_LIBRETRO_UZEM

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


ifeq ($(BR2_PACKAGE_LIBRETRO_UZEM),y)
define CONFIGURE_UZEBOX
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_UZEBOX),Uzebox,$(SYSTEM_NAME_UZEBOX),.uze .UZE .zip .ZIP .7z .7Z,uzebox,uzebox,$(SOURCE_ROMDIR_UZEBOX),$(@D))
endef
RECALBOX_ROMFS_UZEBOX_CONFIGURE_CMDS += $(CONFIGURE_UZEBOX)
endif

$(eval $(generic-package))
