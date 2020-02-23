################################################################################
#
# recalbox-romfs-amiga600
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system amiga600 --extension '.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9 .dms .DMS .fdi .FDI .hdf .HDF .hdz .HDZ .m3u .M3U' --fullname 'Amiga 600' --platform amiga --theme amiga600 1:amiberry:amiberry:BR2_PACKAGE_AMIBERRY 2:libretro:puae:BR2_PACKAGE_LIBRETRO_UAE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_AMIGA600_SOURCE = 
RECALBOX_ROMFS_AMIGA600_SITE = 
RECALBOX_ROMFS_AMIGA600_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_AMIGA600 = amiga600
SYSTEM_XML_AMIGA600 = $(@D)/$(SYSTEM_NAME_AMIGA600).xml
# System rom path
SOURCE_ROMDIR_AMIGA600 = $(RECALBOX_ROMFS_AMIGA600_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_AMIBERRY)$(BR2_PACKAGE_LIBRETRO_UAE),)
define CONFIGURE_MAIN_AMIGA600_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_AMIGA600),Amiga 600,$(SYSTEM_NAME_AMIGA600),.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9 .dms .DMS .fdi .FDI .hdf .HDF .hdz .HDZ .m3u .M3U,amiga,amiga600)
endef

ifneq ($(BR2_PACKAGE_AMIBERRY)$(BR2_PACKAGE_LIBRETRO_UAE),)
define CONFIGURE_AMIGA600_AMIBERRY_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_AMIGA600),amiberry)
endef
ifeq ($(BR2_PACKAGE_AMIBERRY),y)
define CONFIGURE_AMIGA600_AMIBERRY_AMIBERRY_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_AMIGA600),amiberry,1)
endef
endif

define CONFIGURE_AMIGA600_AMIBERRY_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_AMIGA600))
endef
endif

ifneq ($(BR2_PACKAGE_AMIBERRY)$(BR2_PACKAGE_LIBRETRO_UAE),)
define CONFIGURE_AMIGA600_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_AMIGA600),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_UAE),y)
define CONFIGURE_AMIGA600_LIBRETRO_PUAE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_AMIGA600),puae,2)
endef
endif

define CONFIGURE_AMIGA600_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_AMIGA600))
endef
endif



define CONFIGURE_MAIN_AMIGA600_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_AMIGA600),$(SOURCE_ROMDIR_AMIGA600),$(@D))
endef
endif

define RECALBOX_ROMFS_AMIGA600_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_AMIGA600_START)
	$(CONFIGURE_AMIGA600_AMIBERRY_START)
	$(CONFIGURE_AMIGA600_AMIBERRY_AMIBERRY_DEF)
	$(CONFIGURE_AMIGA600_AMIBERRY_END)
	$(CONFIGURE_AMIGA600_LIBRETRO_START)
	$(CONFIGURE_AMIGA600_LIBRETRO_PUAE_DEF)
	$(CONFIGURE_AMIGA600_LIBRETRO_END)
	$(CONFIGURE_MAIN_AMIGA600_END)
endef

$(eval $(generic-package))
