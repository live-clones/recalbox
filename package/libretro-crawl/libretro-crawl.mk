################################################################################
#
# Dungeon Crawl Stone Soup
#
################################################################################

LIBRETRO_CRAWL_VERSION = cc2530f377a3a5d686d4b6a360d88b01de0fbc59
LIBRETRO_CRAWL_SITE = git://github.com/libretro/crawl-ref.git
LIBRETRO_CRAWL_DEPENDENCIES = recalbox-romfs2
LIBRETRO_CRAWL_LICENSE = GPL-2.0+, MIT, BSD-2-clause, BSD-3-clause, Public domain, ZLIB
LIBRETRO_CRAWL_LICENSE_FILES = crawl-ref/license.txt

LIBRETRO_CRAWL_SITE_METHOD=git
LIBRETRO_CRAWL_GIT_SUBMODULES=y

LIBRETRO_CRAWL_ADDITIONAL_CFLAGS = -DPNG_ARM_NEON_OPT=0
ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64)$(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
LIBRETRO_CRAWL_LTO_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_CRAWL_LTO_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_CRAWL_LTO_LDFLAGS = $(COMPILER_COMMONS_LSFLAGS_NOLTO)
else
LIBRETRO_CRAWL_LTO_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_CRAWL_LTO_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_CRAWL_LTO_LDFLAGS = $(COMPILER_COMMONS_LSFLAGS_SO)
endif

define LIBRETRO_CRAWL_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/crawl-ref/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_CRAWL_LTO_CFLAGS) $(LIBRETRO_CRAWL_ADDITIONAL_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_CRAWL_LTO_CXXFLAGS) $(LIBRETRO_CRAWL_ADDITIONAL_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_CRAWL_LTO_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/crawl-ref/ -f Makefile.libretro platform="unix" TILES=y
endef

define LIBRETRO_CRAWL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/crawl-ref/stonesoup_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stonesoup_libretro.so
	mkdir -p "$(TARGET_DIR)/recalbox/share_init/roms/ports/Dungeon Crawl Stone Soup/dat"
	cp -R $(@D)/crawl-ref/source/dat/* "$(TARGET_DIR)/recalbox/share_init/roms/ports/Dungeon Crawl Stone Soup/dat"
endef

$(eval $(generic-package))
