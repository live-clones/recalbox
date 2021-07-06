################################################################################
#
# recalbox-hardware
#
################################################################################

RECALBOX_HARDWARE_VERSION = 25b41d28aaa65991f7539d27e5509460793cb54c
RECALBOX_HARDWARE_SITE = https://gitlab.com/recalbox/recalbox-hardware
RECALBOX_HARDWARE_LICENSE = MIT
RECALBOX_HARDWARE_LICENSE_FILES = LICENSE.md
RECALBOX_HARDWARE_SITE_METHOD = git
RECALBOX_HARDWARE_DEPENDENCIES = recalbox-themes host-python3

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
