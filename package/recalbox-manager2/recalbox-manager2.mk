################################################################################
#
# RECALBOX_MANAGER2
#
################################################################################

RECALBOX_MANAGER2_VERSION = cbe2c2bfb796e40419c126639c22105be86b1f18
RECALBOX_MANAGER2_SITE = https://gitlab.com/recalbox/recalbox-manager
RECALBOX_MANAGER2_LICENSE = COPYRIGHT
RECALBOX_MANAGER2_NON_COMMERCIAL = y

RECALBOX_MANAGER2_SITE_METHOD = git

RECALBOX_MANAGER2_DEPENDENCIES = nodejs

define RECALBOX_MANAGER2_BUILD_CMDS
	$(NPM) --prefix $(@D) run installboth
	$(NPM) --prefix $(@D) run buildboth
	rm -rf $(@D)/release
	mkdir -p $(@D)/release/config
	mkdir -p $(@D)/release/client
	mkdir -p $(@D)/release/locales
	cp -R $(@D)/client/build $(@D)/release/client
	find $(@D) -type f -name '*.map' -exec rm {} \;
	cp $(@D)/config/default.js $(@D)/release/config
	cp $(@D)/config/production.js $(@D)/release/config
	cp -R $(@D)/dist $(@D)/release
	cp $(@D)/locales/*.json $(@D)/release/locales
	cp $(@D)/package.json $(@D)/release
	$(NPM) install --production $(@D)/release/ --prefix $(@D)/release/
endef

define RECALBOX_MANAGER2_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/recalbox-manager2
	cp -r $(@D)/release/* $(TARGET_DIR)/usr/recalbox-manager2
endef

$(eval $(generic-package))
