################################################################################
#
# Dungeon Crawl Stone Soup
#
################################################################################

LIBRETRO_DCSS_VERSION = cc2530f377a3a5d686d4b6a360d88b01de0fbc59
LIBRETRO_DCSS_SITE = git://github.com/libretro/crawl-ref.git
LIBRETRO_DCSS_SITE_METHOD=git
LIBRETRO_DCSS_GIT_SUBMODULES=y

LIBRETRO_DCSS_ADDITIONAL_CFLAGS = -DPNG_ARM_NEON_OPT=0

define LIBRETRO_DCSS_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/crawl-ref/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO) $(LIBRETRO_DCSS_ADDITIONAL_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) $(LIBRETRO_DCSS_ADDITIONAL_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/crawl-ref/ -f Makefile.libretro platform="unix" TILES=y
endef

define LIBRETRO_DCSS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/crawl-ref/stonesoup_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stonesoup_libretro.so
endef

$(eval $(generic-package))
