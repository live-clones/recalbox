################################################################################
#
# tft-waveshare
#
################################################################################

TFT_WAVESHARE_VERSION = e70eb416cae53351f703075b0b40815ff93ccdbf
#TFT_WAVESHARE_SITE = https://github.com/swkim01/waveshare-dtoverlays.git
TFT_WAVESHARE_SITE = $(call github,swkim01,waveshare-dtoverlays,$(TFT_WAVESHARE_VERSION))
TFT_WAVESHARE_LICENSE = COPYRIGHT
TFT_WAVESHARE_NON_COMMERCIAL = y

TFT_WAVESHARE_PI_DEPENDENCIES += rpi-firmware

define TFT_WAVESHARE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/oled-ssd1306.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/oled-ssd1306.dtbo
	$(INSTALL) -D -m 0644 $(@D)/oled-ssd1306.dts $(BINARIES_DIR)/rpi-firmware/overlays/oled-ssd1306.dts
	$(INSTALL) -D -m 0644 $(@D)/oled-ssd1331.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/oled-ssd1331.dtbo
	$(INSTALL) -D -m 0644 $(@D)/oled-ssd1331.dts $(BINARIES_DIR)/rpi-firmware/overlays/oled-ssd1331.dts
	$(INSTALL) -D -m 0644 $(@D)/waveshare32b.dtb $(BINARIES_DIR)/rpi-firmware/overlays/waveshare32b.dtbo
	$(INSTALL) -D -m 0644 $(@D)/waveshare32b.dts $(BINARIES_DIR)/rpi-firmware/overlays/waveshare32b.dts
	$(INSTALL) -D -m 0644 $(@D)/waveshare35a.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35a.dtbo
	$(INSTALL) -D -m 0644 $(@D)/waveshare35a.dts $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35a.dts
	$(INSTALL) -D -m 0644 $(@D)/waveshare35b-v2.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35b-v2.dtbo
	$(INSTALL) -D -m 0644 $(@D)/waveshare35b-v2.dts $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35b-v2.dts
	$(INSTALL) -D -m 0644 $(@D)/waveshare35b.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35b.dtbo
	$(INSTALL) -D -m 0644 $(@D)/waveshare35b.dts $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35b.dts
	$(INSTALL) -D -m 0644 $(@D)/waveshare35c.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35c.dtbo
	$(INSTALL) -D -m 0644 $(@D)/waveshare35c.dts $(BINARIES_DIR)/rpi-firmware/overlays/waveshare35c.dts
endef

$(eval $(generic-package))
