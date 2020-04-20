################################################################################
#
# recalbox-romfs-ports
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system ports --extension '.port .PORT' --fullname 'Ports' --platform pc --theme ports 1:libretro:mrboom:BR2_PACKAGE_LIBRETRO_MRBOOM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PORTS_SOURCE = 
RECALBOX_ROMFS_PORTS_SITE = 
RECALBOX_ROMFS_PORTS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PORTS = ports
SYSTEM_XML_PORTS = $(@D)/$(SYSTEM_NAME_PORTS).xml
# System rom path
SOURCE_ROMDIR_PORTS = $(RECALBOX_ROMFS_PORTS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),)
define CONFIGURE_MAIN_PORTS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PORTS),Ports,$(SYSTEM_NAME_PORTS),.port .PORT,ports,ports)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),)
define CONFIGURE_PORTS_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PORTS),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MRBOOM),y)
define CONFIGURE_PORTS_LIBRETRO_MRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PORTS),mrboom,1)
endef
endif

define CONFIGURE_PORTS_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PORTS))
endef
endif



define CONFIGURE_MAIN_PORTS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PORTS),$(SOURCE_ROMDIR_PORTS),$(@D))
endef
endif

define RECALBOX_ROMFS_PORTS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PORTS_START)
	$(CONFIGURE_PORTS_LIBRETRO_START)
	$(CONFIGURE_PORTS_LIBRETRO_MRBOOM_DEF)
	$(CONFIGURE_PORTS_LIBRETRO_END)
	$(CONFIGURE_MAIN_PORTS_END)
endef

$(eval $(generic-package))
