################################################################################
#
# recalbox-romfs-amiga1200
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system amiga1200 --extension '.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9' --fullname 'Amiga 1200' --platform amiga --theme amiga1200 BR2_PACKAGE_AMIBERRY

# Name the 3 vars as the package requires
RECALBOX_ROMFS_AMIGA1200_SOURCE = 
RECALBOX_ROMFS_AMIGA1200_SITE = 
RECALBOX_ROMFS_AMIGA1200_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_AMIGA1200 = amiga1200
SYSTEM_XML_AMIGA1200 = $(@D)/$(SYSTEM_NAME_AMIGA1200).xml
# System rom path
SOURCE_ROMDIR_AMIGA1200 = $(RECALBOX_ROMFS_AMIGA1200_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_AMIBERRY),y)
define CONFIGURE_AMIGA1200
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_AMIGA1200),Amiga 1200,$(SYSTEM_NAME_AMIGA1200),.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9,amiga,amiga1200,$(SOURCE_ROMDIR_AMIGA1200),$(@D))
endef
RECALBOX_ROMFS_AMIGA1200_CONFIGURE_CMDS += $(CONFIGURE_AMIGA1200)
endif

$(eval $(generic-package))
