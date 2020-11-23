################################################################################
#
# wsd
#
################################################################################

WSD_VERSION = 3b2a6a476b47822e7231a0f209c725489da50801
WSD_SITE = $(call github,Andy2244,wsdd2,$(WSD_VERSION))
WSD_LICENSE = GPL3
WSD_LICENSE_FILES = LICENSE

define WSD_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile
endef

define WSD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/wsdd2 \
		$(TARGET_DIR)/usr/sbin/wsdd2
	$(INSTALL) -D -m 755 $(WSD_PKGDIR)/S92wsd \
		$(TARGET_DIR)/etc/init.d/S92wsd
endef

$(eval $(generic-package))
