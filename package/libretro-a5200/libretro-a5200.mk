################################################################################
#
# A5200
#
################################################################################

LIBRETRO_A5200_VERSION = 599a2e53feaf47c920bbb0f1f419cf54af570d58
LIBRETRO_A5200_SITE = $(call github,libretro,a5200,$(LIBRETRO_A5200_VERSION))
LIBRETRO_A5200_LICENSE = GPL

define LIBRETRO_A5200_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_A5200_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/a5200_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/a5200_libretro.so
endef

$(eval $(generic-package))
