################################################################################
#
# PAMIX
#
################################################################################

PAMIX_VERSION = ec3a38372803fb1fd4c5ce8f70fb196b5df9ce89
PAMIX_SITE = $(call github,patroclos,PAmix,$(PAMIX_VERSION))
PAMIX_LICENSE = MIT
PAMIX_LICENSE_FILES = LICENSE
PAMIX_DEPENDENCIES = pulseaudio ncurses
PAMIX_CONF_OPTS = -DCMAKE_BUILD_TYPE=RELEASE -DWITH_UNICODE=0

define PAMIX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/pamix $(TARGET_DIR)/usr/bin/pamix
	$(INSTALL) -D $(@D)/pamix.conf $(TARGET_DIR)/etc/xdg/pamix.conf
endef

$(eval $(cmake-package))
