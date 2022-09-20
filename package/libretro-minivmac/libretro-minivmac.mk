################################################################################
#
# MINIVMAC
#
################################################################################

# Commit of 2022-07-30
LIBRETRO_MINIVMAC_VERSION = 7a774cdb3e754c65f4c1df622cea93a53f7ab9f7
LIBRETRO_MINIVMAC_SITE = https://github.com/libretro/libretro-minivmac.git
LIBRETRO_MINIVMAC_SITE_METHOD = git
LIBRETRO_MINIVMAC_LICENSE = GPL-1.0
LIBRETRO_MINIVMAC_GIT_SUBMODULES=y

define LIBRETRO_MINIVMAC_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_MINIVMAC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/minivmac_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/minivmac_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/macintosh
endef

$(eval $(generic-package))
