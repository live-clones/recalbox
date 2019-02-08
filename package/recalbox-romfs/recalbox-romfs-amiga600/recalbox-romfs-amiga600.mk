################################################################################
#
# recalbox-romfs-amiga600
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system amiga600 --extension '.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9' --fullname 'Amiga 600' --platform amiga --theme amiga600 BR2_PACKAGE_AMIBERRY

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


ifeq ($(BR2_PACKAGE_AMIBERRY),y)
define CONFIGURE_AMIGA600
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_AMIGA600),Amiga 600,$(SYSTEM_NAME_AMIGA600),.adf .Adf .ADF .ipf .IPF .Ipf .lha .LHA .lhz .LHZ .lzx .LZX .zip .ZIP .rp9 .RP9,amiga,amiga600,$(SOURCE_ROMDIR_AMIGA600),$(@D))
endef
RECALBOX_ROMFS_AMIGA600_CONFIGURE_CMDS += $(CONFIGURE_AMIGA600)
endif

$(eval $(generic-package))
