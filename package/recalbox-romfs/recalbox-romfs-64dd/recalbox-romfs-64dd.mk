################################################################################
#
# recalbox-romfs-64dd
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system 64dd --extension '.n64 .N64 .z64 .Z64 .v64 .V64 .bin .BIN .u1 .U1 .ndd .NDD' --fullname 'Nintendo 64DD' --platform 64dd --theme 64dd 1:libretro:parallel_n64:BR2_PACKAGE_LIBRETRO_PARALLEL_N64 2:libretro:mupen64plus_next:BR2_PACKAGE_LIBRETRO_MUPEN64PLUS_NX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_64DD_SOURCE = 
RECALBOX_ROMFS_64DD_SITE = 
RECALBOX_ROMFS_64DD_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_64DD = 64dd
SYSTEM_XML_64DD = $(@D)/$(SYSTEM_NAME_64DD).xml
# System rom path
SOURCE_ROMDIR_64DD = $(RECALBOX_ROMFS_64DD_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PARALLEL_N64),)
define CONFIGURE_MAIN_64DD_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_64DD),Nintendo 64DD,$(SYSTEM_NAME_64DD),.n64 .N64 .z64 .Z64 .v64 .V64 .bin .BIN .u1 .U1 .ndd .NDD,64dd,64dd)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PARALLEL_N64),)
define CONFIGURE_64DD_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_64DD),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PARALLEL_N64),y)
define CONFIGURE_64DD_LIBRETRO_PARALLEL_N64_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_64DD),parallel_n64,1)
endef
endif

define CONFIGURE_64DD_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_64DD))
endef
endif



define CONFIGURE_MAIN_64DD_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_64DD),$(SOURCE_ROMDIR_64DD),$(@D))
endef
endif

define RECALBOX_ROMFS_64DD_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_64DD_START)
	$(CONFIGURE_64DD_LIBRETRO_START)
	$(CONFIGURE_64DD_LIBRETRO_PARALLEL_N64_DEF)
	$(CONFIGURE_64DD_LIBRETRO_END)
	$(CONFIGURE_MAIN_64DD_END)
endef

$(eval $(generic-package))
