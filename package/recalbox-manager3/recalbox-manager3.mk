################################################################################
#
# RECALBOX_MANAGER3
#
################################################################################

ifneq ($(GITLAB_TOKEN_WEBMANAGER3),)
RECALBOX_MANAGER3_VERSION = 53a14a166c6261fec8134186fccf15c8fa7099a2
RECALBOX_MANAGER3_SITE = https://gitlab-ci-token:$(GITLAB_TOKEN_WEBMANAGER3)@gitlab.com/recalbox/recalbox-manager-v3.git
else
RECALBOX_MANAGER3_VERSION = b1f66e32d41a84a9e05786b4c0b5ee4bc9783922
RECALBOX_MANAGER3_SITE = https://gitlab.com/Bkg2k/recalbox-manager-v3-empty.git
endif
RECALBOX_MANAGER3_SITE_METHOD = git

define RECALBOX_MANAGER3_BUILD_CMDS
	if [ -f $(@D)/package-lock.json ] ; then rm $(@D)/package-lock.json ; fi
	cd $(@D) && ls && \
		HOME=/tmp yarn install && \
		HOME=/tmp quasar build
endef

define RECALBOX_MANAGER3_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/web/manager-v3
	cp -r $(@D)/dist/spa/* \
		$(TARGET_DIR)/recalbox/web/manager-v3
endef

$(eval $(generic-package))
