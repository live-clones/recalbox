################################################################################
#
# XPADNEO
#
################################################################################

XPADNEO_VERSION = v0.9.1
XPADNEO_SITE = https://github.com/atar-axis/xpadneo
XPADNEO_SITE_METHOD = git
XPADNEO_LICENSE = GPL-3.0
XPADNEO_MODULE_SUBDIRS = hid-xpadneo/src

define XPADNEO_COPY_CMDS
	$(INSTALL) -D -m 0644 -t $(TARGET_DIR)/etc/modprobe.d $(@D)/hid-xpadneo/etc-modprobe.d/xpadneo.conf
	$(INSTALL) -D -m 0644 -t $(TARGET_DIR)/etc/udev/rules.d $(@D)/hid-xpadneo/etc-udev-rules.d/60-xpadneo.rules
endef

XPADNEO_INSTALL_TARGET_CMDS += $(XPADNEO_COPY_CMDS)

$(eval $(kernel-module))
$(eval $(generic-package))
