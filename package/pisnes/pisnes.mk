################################################################################
#
# PISNES
#
################################################################################

PISNES_VERSION = 2335271b8da5724df14d3cb3735c7786b834eaec
PISNES_SITE = https://gitlab.com/Bkg2k/pisnes
PISNES_SITE_METHOD = git

PISNES_LICENSE = COPYRIGHT
PISNES_NON_COMMERCIAL = y

define PISNES_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_EXE)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_EXE)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_EXE)" \
		STAGING_ROOT="$(STAGING_DIR)" \
		$(MAKE) CCC="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D) -f Makefile
endef

define PISNES_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/snes9x \
		$(TARGET_DIR)/usr/bin/pisnes
endef

$(eval $(generic-package))
