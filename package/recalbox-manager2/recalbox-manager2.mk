################################################################################
#
# RECALBOX_MANAGER2
#
################################################################################
# RECALBOX_MANAGER2_VERSION = v1.0-beta6
RECALBOX_MANAGER2_VERSION = v2.0-beta
RECALBOX_MANAGER2_SITE = $(call github,djlechuck,recalboxmanager,$(RECALBOX_MANAGER2_VERSION))
RECALBOX_MANAGER2_SITE = git://github.com/DjLeChuck/recalbox-manager
RECALBOX_MANAGER2_DEPENDENCIES = nodejs

# NPM = $(TARGET_CONFIGURE_OPTS) \
	# LD="$(TARGET_CXX)" \
	# npm_config_arch=arm \
	# npm_config_target_arch=arm \
	# npm_config_build_from_source=true \
	# npm_config_nodedir=$(BUILD_DIR)/nodejs-$(NODEJS_VERSION) \
	# $(HOST_DIR)/usr/bin/npm

define RECALBOX_MANAGER2_BUILD_CMDS		
	$(NPM) --prefix $(@D) run installboth
	$(NPM) --prefix $(@D) run buildboth
	rm -rf $(@D)/release
	mkdir -p $(@D)/release/config
	mkdir -p $(@D)/release/client
	cp -R $(@D)/client/build $(@D)/release/client
	find $(@D) -type f -name '*.map' -exec rm {} \;
	cp $(@D)/config/default.js $(@D)/release/config
	cp $(@D)/config/production.js $(@D)/release/config
	cp -R $(@D)/dist $(@D)/release
	cp $(@D)/package.json $(@D)/release
	$(NPM) install --production $(@D)/release/ --prefix $(@D)/release/
endef
    
define RECALBOX_MANAGER2_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/recalbox-manager2
	cp -r $(@D)/release/* $(TARGET_DIR)/usr/recalbox-manager2
endef


$(eval $(generic-package))
