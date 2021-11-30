################################################################################
#
# eepromutils
#
################################################################################

EEPROMUTILS_VERSION = c8a2f630f9d50159d94ad6ad7c24973cdeb7b8f1
EEPROMUTILS_SITE = $(call github,raspberrypi,hats,$(EEPROMUTILS_VERSION))
EEPROMUTILS_LICENSE = GPL2

define EEPROMUTILS_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) PREFIX="$(STAGING_DIR)/usr" \
		PKG_CONFIG="$(HOST_DIR)/usr/bin/pkg-config" \
		CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/eepromutils
endef

define EEPROMUTILS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0544 $(@D)/eepromutils/eepdump $(TARGET_DIR)/usr/bin/
	$(INSTALL) -D -m 0544 $(@D)/eepromutils/eepflash.sh $(TARGET_DIR)/usr/bin/
	$(INSTALL) -D -m 0544 $(@D)/eepromutils/eepmake $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
