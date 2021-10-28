################################################################################
#
# UAE4ALL
#
################################################################################

LIBRETRO_UAE4ALL_VERSION = ae7eb74cfbe726a45f5b791484948676dcba1aef
LIBRETRO_UAE4ALL_SITE = $(call github,Chips-fr,uae4all-rpi,$(LIBRETRO_UAE4ALL_VERSION))
LIBRETRO_UAE4ALL_BRANCH = master
LIBRETRO_UAE4ALL_LICENSE = GPL-2.0
LIBRETRO_UAE4ALL_LICENSE_FILES = COPYING
#LIBRETRO_UAE4ALL_DEPENDENCIES = flac mpg123

LIBRETRO_UAE4ALL_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_UAE4ALL_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_UAE4ALL_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

define LIBRETRO_UAE4ALL_BUILD_CMDS
	    CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_UAE4ALL_COMPILER_COMMONS_CFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AS="$(TARGET_AS)" -C $(@D)/ -f Makefile.libretro
endef

define LIBRETRO_UAE4ALL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/uae4all_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/uae4all_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/uae_data
endef

$(eval $(generic-package))
