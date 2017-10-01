################################################################################
#
# VIRTUALGAMEPADS
#
################################################################################
VIRTUALGAMEPADS_VERSION = 6843236600010f611d96f047e19902cd5de0e12c
VIRTUALGAMEPADS_BRANCH = recalbox
VIRTUALGAMEPADS_SITE = $(call github,miroof,node-virtual-gamepads,$(VIRTUALGAMEPADS_BRANCH),$(VIRTUALGAMEPADS_VERSION))
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
		$(NPM) install \

endef


define VIRTUALGAMEPADS_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/virtualgamepads/S92virtualgamepads $(TARGET_DIR)/etc/init.d/S92virtualgamepads
endef
$(eval $(generic-package))
