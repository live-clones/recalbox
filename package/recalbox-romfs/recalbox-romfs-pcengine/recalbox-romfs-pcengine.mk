################################################################################
#
# recalbox-romfs-pcengine
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system pcengine --extension '.pce .PCE .cue .CUE .sgx .SGX .zip .ZIP .ccd .CCD' --fullname 'PC Engine' --platform pcengine --theme pcengine libretro:mednafen_supergrafx:BR2_PACKAGE_LIBRETRO_BEETLE_SUPERGRAFX libretro:pce:BR2_PACKAGE_LIBRETRO_BEETLE_PCE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PCENGINE_SOURCE = 
RECALBOX_ROMFS_PCENGINE_SITE = 
RECALBOX_ROMFS_PCENGINE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PCENGINE = pcengine
SYSTEM_XML_PCENGINE = $(@D)/$(SYSTEM_NAME_PCENGINE).xml
# System rom path
SOURCE_ROMDIR_PCENGINE = $(RECALBOX_ROMFS_PCENGINE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_SUPERGRAFX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PCE),)
define CONFIGURE_MAIN_PCENGINE_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PCENGINE),PC Engine,$(SYSTEM_NAME_PCENGINE),.pce .PCE .cue .CUE .sgx .SGX .zip .ZIP .ccd .CCD,pcengine,pcengine)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_SUPERGRAFX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PCE),)
define CONFIGURE_PCENGINE_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PCENGINE),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_SUPERGRAFX),y)
define CONFIGURE_PCENGINE_LIBRETRO_MEDNAFEN_SUPERGRAFX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PCENGINE),mednafen_supergrafx)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PCE),y)
define CONFIGURE_PCENGINE_LIBRETRO_PCE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PCENGINE),pce)
endef
endif

define CONFIGURE_PCENGINE_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PCENGINE))
endef
endif



define CONFIGURE_MAIN_PCENGINE_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PCENGINE),$(SOURCE_ROMDIR_PCENGINE),$(@D))
endef
endif

define RECALBOX_ROMFS_PCENGINE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PCENGINE_START)
	$(CONFIGURE_PCENGINE_LIBRETRO_START)
	$(CONFIGURE_PCENGINE_LIBRETRO_MEDNAFEN_SUPERGRAFX_DEF)
	$(CONFIGURE_PCENGINE_LIBRETRO_PCE_DEF)
	$(CONFIGURE_PCENGINE_LIBRETRO_END)
	$(CONFIGURE_MAIN_PCENGINE_END)
endef

$(eval $(generic-package))
