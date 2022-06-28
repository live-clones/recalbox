################################################################################
#
# recalbox-hardware
#
################################################################################

RECALBOX_HARDWARE_VERSION = custom
RECALBOX_HARDWARE_SITE = $(TOPDIR)/../projects/recalbox-hardware
RECALBOX_HARDWARE_SITE_METHOD = local
RECALBOX_HARDWARE_LICENSE = MIT
RECALBOX_HARDWARE_LICENSE_FILES = LICENSE.md
RECALBOX_HARDWARE_DEPENDENCIES = recalbox-themes host-python3 linux

define RECALBOX_HARDWARE_BUILD_PI
	$(HOST_DIR)/bin/linux-dtc $(@D)/case/installers/retroflags/assets/overlays/retroflag-case.dts -o $(@D)/retroflag-case.dtbo
	$(INSTALL) -D -m 0644 $(@D)/retroflag-case.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/retroflag-case.dtbo
endef

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
RECALBOX_HARDWARE_POST_BUILD_HOOKS += RECALBOX_HARDWARE_BUILD_PI
endif

define RECALBOX_HARDWARE_BUILD_CMDS
	PYTHONPATH="$(PYTHON3_PATH)" $(HOST_DIR)/bin/python$(PYTHON3_VERSION_MAJOR) -m compileall $(@D)
endef

define RECALBOX_HARDWARE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/system/hardware
	cp -r $(@D)/* \
		$(TARGET_DIR)/recalbox/system/hardware/
	cp -r $(RECALBOX_THEMES_BUILDDIR)/themes/recalbox-gpicase \
		$(TARGET_DIR)/recalbox/system/hardware/case/installers/gpi/assets/theme
	# Please replace me with package dependency & file copy
	wget -P $(TARGET_DIR)/recalbox/system/hardware/case/installers/gpi/assets/overlays/ \
		https://github.com/ian57/dpi-pizero-gpicase/raw/master/dpi-pizero-gpicase.dtbo
	wget -P $(TARGET_DIR)/recalbox/system/hardware/case/installers/gpi/assets/overlays/ \
		https://github.com/ian57/pwm-audio-pizero-gpicase/raw/master/pwm-audio-pizero-gpicase.dtbo
endef

$(eval $(generic-package))
