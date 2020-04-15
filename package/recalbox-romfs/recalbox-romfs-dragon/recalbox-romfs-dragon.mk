################################################################################
#
# recalbox-romfs-dragon
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system dragon --extension '.cas .CAS .wav .WAV .bas .BAS .asc .ASC .dmk .DMK .jvc .JVC .os9 .OS9 .dsk .DSK .vdk .VDK .bin .BIN .hex .HEX' --fullname 'Dragon 32/64' --platform dragon --theme dragon BR2_PACKAGE_XROAR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DRAGON_SOURCE = 
RECALBOX_ROMFS_DRAGON_SITE = 
RECALBOX_ROMFS_DRAGON_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DRAGON = dragon
SYSTEM_XML_DRAGON = $(@D)/$(SYSTEM_NAME_DRAGON).xml
# System rom path
SOURCE_ROMDIR_DRAGON = $(RECALBOX_ROMFS_DRAGON_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_XROAR),y)
define CONFIGURE_DRAGON
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_DRAGON),Dragon 32/64,$(SYSTEM_NAME_DRAGON),.cas .CAS .wav .WAV .bas .BAS .asc .ASC .dmk .DMK .jvc .JVC .os9 .OS9 .dsk .DSK .vdk .VDK .bin .BIN .hex .HEX,dragon,dragon,$(SOURCE_ROMDIR_DRAGON),$(@D))
endef
RECALBOX_ROMFS_DRAGON_CONFIGURE_CMDS += $(CONFIGURE_DRAGON)
endif

$(eval $(generic-package))

