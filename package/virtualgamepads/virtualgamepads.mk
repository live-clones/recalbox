################################################################################
#
# VIRTUALGAMEPADS
#
################################################################################

VIRTUALGAMEPADS_VERSION = v1.4.0
VIRTUALGAMEPADS_BRANCH = recalbox
VIRTUALGAMEPADS_SITE = $(call github,jehervy,node-virtual-gamepads,$(VIRTUALGAMEPADS_BRANCH),$(VIRTUALGAMEPADS_VERSION))
VIRTUALGAMEPADS_LICENSE = MIT
VIRTUALGAMEPADS_LICENSE_FILES = LICENSE

VIRTUALGAMEPADS_DEPENDENCIES = nodejs

NPM = $(TARGET_CONFIGURE_OPTS) \
	LD="$(TARGET_CXX)" \
	npm_config_arch=arm \
	npm_config_target_arch=arm \
	npm_config_build_from_source=true \
	npm_config_nodedir=$(BUILD_DIR)/nodejs-$(NODEJS_VERSION) \
	$(HOST_DIR)/usr/bin/npm

define VIRTUALGAMEPADS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/node-virtual-gamepads
	cp -r $(@D)/* \
		$(TARGET_DIR)/usr/node-virtual-gamepads

	mv $(TARGET_DIR)/usr/node-virtual-gamepads/main.js $(TARGET_DIR)/usr/node-virtual-gamepads/virtualgamepads.js

	cd $(TARGET_DIR)/usr/node-virtual-gamepads && mkdir -p node_modules && \
		$(NPM) install
endef

define VIRTUALGAMEPADS_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D $(VIRTUALGAMEPADS_PKGDIR)/S92virtualgamepads $(TARGET_DIR)/etc/init.d/S92virtualgamepads
endef

$(eval $(generic-package))
