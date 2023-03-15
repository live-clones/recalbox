################################################################################
#
# Arcade dats processor
#
################################################################################

ARCADE_DATS_VERSION = 252
ARCADE_DATS_SOURCE = MAME_Dats_$(ARCADE_DATS_VERSION).7z
ARCADE_DATS_SITE = https://progettosnaps.net/dats/MAME/packs
ARCADE_DATS_LICENSE = MAME

ARCADE_DATS_FULLARCADE_DAT = $(ARCADE_DATS_DIR)/mame$(ARCADE_DATS_VERSION).xml
ARCADE_DATS_FULLARCADE_ARCADEDAT = '$(ARCADE_DATS_DIR)/MAME $(ARCADE_DATS_VERSION) (Arcade).dat'

define ARCADE_DATS_EXTRACT_CMDS
	cp $(ARCADE_DATS_PKGDIR)/*.xslt $(ARCADE_DATS_DIR)/
	7z e -o$(ARCADE_DATS_DIR)/ $(DL_DIR)/arcade-dats/$(ARCADE_DATS_SOURCE)
endef

define ARCADE_DATS_BUILD_CMDS
endef

define ARCADE_DATS_INSTALL_TARGET_CMDS
endef

$(eval $(generic-package))
