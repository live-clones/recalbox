################################################################################
#
# recalbox-romfs-pc88
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pc88 --extension '.d88 .D88 .t88 .T88 .cmt .CMT .zip .ZIP .7z .7Z' --fullname 'NEC PC-88' --platform pc88 --theme pc88 1:libretro:quasi88:BR2_PACKAGE_LIBRETRO_QUASI88

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PC88_SOURCE = 
RECALBOX_ROMFS_PC88_SITE = 
RECALBOX_ROMFS_PC88_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PC88 = pc88
SYSTEM_XML_PC88 = $(@D)/$(SYSTEM_NAME_PC88).xml
# System rom path
SOURCE_ROMDIR_PC88 = $(RECALBOX_ROMFS_PC88_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_QUASI88),)
define CONFIGURE_MAIN_PC88_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PC88),NEC PC-88,$(SYSTEM_NAME_PC88),.d88 .D88 .t88 .T88 .cmt .CMT .zip .ZIP .7z .7Z,pc88,pc88)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_QUASI88),)
define CONFIGURE_PC88_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PC88),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_QUASI88),y)
define CONFIGURE_PC88_LIBRETRO_QUASI88_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PC88),quasi88,1)
endef
endif

define CONFIGURE_PC88_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PC88))
endef
endif



define CONFIGURE_MAIN_PC88_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PC88),$(SOURCE_ROMDIR_PC88),$(@D))
endef
endif

define RECALBOX_ROMFS_PC88_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PC88_START)
	$(CONFIGURE_PC88_LIBRETRO_START)
	$(CONFIGURE_PC88_LIBRETRO_QUASI88_DEF)
	$(CONFIGURE_PC88_LIBRETRO_END)
	$(CONFIGURE_MAIN_PC88_END)
endef

$(eval $(generic-package))
