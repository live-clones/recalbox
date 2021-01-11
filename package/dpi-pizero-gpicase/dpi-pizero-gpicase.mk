################################################################################
#
# dpi-pizero-gpicase
#
################################################################################

DPI_PIZERO_GPICASE_VERSION = ac63baf1f394913aa39c0ce216d85c58ff6a143c
DPI_PIZERO_GPICASE_SITE = $(call github,ian57,dpi-pizero-gpicase,$(DPI_PIZERO_GPICASE_VERSION))
DPI_PIZERO_GPICASE_LICENSE = GPL-2.0
DPI_PIZERO_GPICASE_LICENSE_FILES = LICENSE

DPI_PIZERO_GPICASE_DEPENDENCIES += rpi-firmware

define DPI_PIZERO_GPICASE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/dpi-pizero-gpicase.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/dpi-pizero-gpicase.dtbo
	$(INSTALL) -D -m 0644 $(@D)/dpi-pizero-gpicase.dts $(BINARIES_DIR)/rpi-firmware/overlays/dpi-pizero-gpicase.dts
endef

$(eval $(generic-package))
