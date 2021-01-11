################################################################################
#
# FREECHAF
#
################################################################################

LIBRETRO_FREECHAF_VERSION = 923a42b30fcda9b0d2ef84916b7b0e0d1760df2b
LIBRETRO_FREECHAF_SITE = git://github.com/libretro/FreeChaF.git
LIBRETRO_FREECHAF_LICENSE = GPL-3.0
LIBRETRO_FREECHAF_LICENSE_FILES = LICENSE

LIBRETRO_FREECHAF_SITE_METHOD=git
LIBRETRO_FREECHAF_GIT_SUBMODULES=y

define LIBRETRO_FREECHAF_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	# Currently, FreeChaF crash when compiled with LTO.
	# As it might be a compiler or a code issue, try to reactivate LTO
	# on next toolchain or code bump.
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_FREECHAF_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/freechaf_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/freechaf_libretro.so
endef

$(eval $(generic-package))
