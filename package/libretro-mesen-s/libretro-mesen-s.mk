################################################################################
#
# MESEN_S
#
################################################################################

# Commit of 2022/07/25
LIBRETRO_MESEN_S_VERSION = 32a7adfb4edb029324253cb3632dfc6599ad1aa8
LIBRETRO_MESEN_S_SITE = $(call github,libretro,Mesen-S,$(LIBRETRO_MESEN_S_VERSION))
LIBRETRO_MESEN_S_LICENSE = GPL-3.0
LIBRETRO_MESEN_S_LICENSE_FILES = LICENSE

define LIBRETRO_MESEN_S_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/Libretro -f Makefile platform="unix"
endef

define LIBRETRO_MESEN_S_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/Libretro/mesen-s_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mesen_s_libretro.so
endef

$(eval $(generic-package))
