################################################################################
#
# LIBGO2
#
################################################################################

LIBGO2_VERSION = fc78629ffc5152035c5872e609b7468254eae9ec
LIBGO2_SITE = $(call github,OtherCrashOverride,libgo2,$(LIBGO2_VERSION))
LIBGO2_LICENSE = LGPL-2.0
LIBGO2_LICENSE_FILES = LICENSE

LIBGO2_DEPENDENCIES = alsa-lib libevdev libdrm libpng librga openal rockchip-mali
LIBGO2_INSTALL_STAGING = YES

define LIBGO2_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CFXXLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		AS="$(TARGET_AS)" \
		config=release \
		STRIP="$(TARGET_STRIP)" \
	 	-C $(@D) -f Makefile
endef

define LIBGO2_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libgo2.so $(STAGING_DIR)/usr/lib/libgo2.so
	$(INSTALL) -D -m 0644 $(@D)/src/audio.h $(STAGING_DIR)/usr/include/go2/audio.h
	$(INSTALL) -D -m 0644 $(@D)/src/display.h $(STAGING_DIR)/usr/include/go2/display.h
	$(INSTALL) -D -m 0644 $(@D)/src/hardware.h $(STAGING_DIR)/usr/include/go2/hardware.h
	$(INSTALL) -D -m 0644 $(@D)/src/input.h $(STAGING_DIR)/usr/include/go2/input.h
	$(INSTALL) -D -m 0644 $(@D)/src/queue.h $(STAGING_DIR)/usr/include/go2/queue.h
endef

define LIBGO2_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libgo2.so $(TARGET_DIR)/usr/lib/libgo2.so
	$(INSTALL) -D -m 0644 $(@D)/src/audio.h $(TARGET_DIR)/usr/include/go2/audio.h
	$(INSTALL) -D -m 0644 $(@D)/src/display.h $(TARGET_DIR)/usr/include/go2/display.h
	$(INSTALL) -D -m 0644 $(@D)/src/hardware.h $(TARGET_DIR)/usr/include/go2/hardware.h
	$(INSTALL) -D -m 0644 $(@D)/src/input.h $(TARGET_DIR)/usr/include/go2/input.h
	$(INSTALL) -D -m 0644 $(@D)/src/queue.h $(TARGET_DIR)/usr/include/go2/queue.h
endef

$(eval $(generic-package))
