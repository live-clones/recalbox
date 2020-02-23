################################################################################
#
# recalbox-romfs-palm
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system palm --extension '.prc .PRC .pdb .PDB .pqa .PQA .img .IMG' --fullname 'Palm' --platform palm --theme palm 1:libretro:mu:BR2_PACKAGE_LIBRETRO_MU

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PALM_SOURCE = 
RECALBOX_ROMFS_PALM_SITE = 
RECALBOX_ROMFS_PALM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PALM = palm
SYSTEM_XML_PALM = $(@D)/$(SYSTEM_NAME_PALM).xml
# System rom path
SOURCE_ROMDIR_PALM = $(RECALBOX_ROMFS_PALM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_MU),)
define CONFIGURE_MAIN_PALM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PALM),Palm,$(SYSTEM_NAME_PALM),.prc .PRC .pdb .PDB .pqa .PQA .img .IMG,palm,palm)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_MU),)
define CONFIGURE_PALM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PALM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MU),y)
define CONFIGURE_PALM_LIBRETRO_MU_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PALM),mu,1)
endef
endif

define CONFIGURE_PALM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PALM))
endef
endif



define CONFIGURE_MAIN_PALM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PALM),$(SOURCE_ROMDIR_PALM),$(@D))
endef
endif

define RECALBOX_ROMFS_PALM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PALM_START)
	$(CONFIGURE_PALM_LIBRETRO_START)
	$(CONFIGURE_PALM_LIBRETRO_MU_DEF)
	$(CONFIGURE_PALM_LIBRETRO_END)
	$(CONFIGURE_MAIN_PALM_END)
endef

$(eval $(generic-package))