################################################################################
#
# recalbox-romfs-x1
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system x1 --extension '.dx1 .DX1 .2d .2D .2hd .2HD .tfd .TFD .d88 .D88 .88d .88D .hdm .HDM .xdf .XDF .dup .DUP .cmd .CMD .zip .ZIP .7z .7Z' --fullname 'Sharp X1' --platform x1 --theme x1 1:libretro:xmil:BR2_PACKAGE_LIBRETRO_XMIL

# Name the 3 vars as the package requires
RECALBOX_ROMFS_X1_SOURCE = 
RECALBOX_ROMFS_X1_SITE = 
RECALBOX_ROMFS_X1_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_X1 = x1
SYSTEM_XML_X1 = $(@D)/$(SYSTEM_NAME_X1).xml
# System rom path
SOURCE_ROMDIR_X1 = $(RECALBOX_ROMFS_X1_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_XMIL),)
define CONFIGURE_MAIN_X1_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_X1),Sharp X1,$(SYSTEM_NAME_X1),.dx1 .DX1 .2d .2D .2hd .2HD .tfd .TFD .d88 .D88 .88d .88D .hdm .HDM .xdf .XDF .dup .DUP .cmd .CMD .zip .ZIP .7z .7Z,x1,x1)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_XMIL),)
define CONFIGURE_X1_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_X1),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_XMIL),y)
define CONFIGURE_X1_LIBRETRO_XMIL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_X1),xmil,1)
endef
endif

define CONFIGURE_X1_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_X1))
endef
endif



define CONFIGURE_MAIN_X1_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_X1),$(SOURCE_ROMDIR_X1),$(@D))
endef
endif

define RECALBOX_ROMFS_X1_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_X1_START)
	$(CONFIGURE_X1_LIBRETRO_START)
	$(CONFIGURE_X1_LIBRETRO_XMIL_DEF)
	$(CONFIGURE_X1_LIBRETRO_END)
	$(CONFIGURE_MAIN_X1_END)
endef

$(eval $(generic-package))
