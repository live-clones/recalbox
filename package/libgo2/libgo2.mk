################################################################################
#
# LIBGO2
#
################################################################################

LIBGO2_VERSION = fc78629ffc5152035c5872e609b7468254eae9ec
LIBGO2_SITE = $(call github,OtherCrashOverride,libgo2,$(LIBGO2_VERSION))
LIBGO2_LICENSE = LICENSE

LIBGO2_DEPENDENCIES = alsa-lib libevdev libdrm libpng librga openal rockchip-mali
LIBGO2_INSTALL_STAGING = YES

define LIBGO2_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		AS="$(TARGET_AS)" \
		config=release \
		STRIP="$(TARGET_STRIP)" \
	 	-C $(@D) -f Makefile
endef

define LIBGO2_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libgo2.so $(STAGING_DIR)/usr/lib/libgo2.so
endef

define LIBGO2_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libgo2.so $(TARGET_DIR)/usr/lib/libgo2.so
endef

$(eval $(generic-package))
