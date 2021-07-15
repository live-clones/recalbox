################################################################################
#
# switchres
#
################################################################################

SWITCHRES_VERSION = a8af8e8f7f54d24414b4a8595a84c0f0f23e14a1
SWITCHRES_SITE = $(call github,antonioginer,switchres,$(SWITCHRES_VERSION))
SWITCHRES_LICENSE = GPL2
SWITCHRES_DEPENDENCIES = libdrm host-dos2unix

# Hack to patch the CRLF files
define SWITCHRES_CRLF_PATH
	$(HOST_DIR)/usr/bin/dos2unix $(@D)/modeline.cpp $(@D)/switchres.cpp
endef

SWITCHRES_PRE_PATCH_HOOKS += SWITCHRES_CRLF_PATH

define SWITCHRES_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) PREFIX="$(STAGING_DIR)/usr" \
		PKG_CONFIG="$(HOST_DIR)/usr/bin/pkg-config" \
		CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f makefile
endef

define SWITCHRES_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0544 $(@D)/switchres $(TARGET_DIR)/usr/bin/switchres
endef

$(eval $(generic-package))
