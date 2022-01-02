################################################################################
#
# rpi-dtoverlay
#
################################################################################

RPI_DTOVERLAY_VERSION = 45a0022ac64b4d0788def3c5230c972430f6fc23
RPI_DTOVERLAY_SITE = $(call github,raspberrypi,userland,$(RPI_DTOVERLAY_VERSION))
RPI_DTOVERLAY_LICENSE = BSD-3-Clause
RPI_DTOVERLAY_LICENSE_FILES = LICENCE
RPI_DTOVERLAY_INSTALL_STAGING = YES
RPI_DTOVERLAY_CONF_OPTS = -DVMCS_INSTALL_PREFIX=/usr

RPI_DTOVERLAY_CONF_OPTS += -DALL_APPS=ON

define RPI_DTOVERLAY_POST_TARGET_CLEANUP
	rm -Rf $(TARGET_DIR)/usr/src
endef
RPI_DTOVERLAY_POST_INSTALL_TARGET_HOOKS += RPI_DTOVERLAY_POST_TARGET_CLEANUP

define RPI_DTOVERLAY_INSTALL_STAGING_CMDS
	$(INSTALL) -m 0744 -D \
		$(@D)/build/bin/dtoverlay \
		$(STAGING_DIR)/usr/bin/dtoverlay
endef

define RPI_DTOVERLAY_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0744 -D \
		$(@D)/build/bin/dtoverlay \
		$(TARGET_DIR)/usr/bin/dtoverlay
endef

$(eval $(cmake-package))
