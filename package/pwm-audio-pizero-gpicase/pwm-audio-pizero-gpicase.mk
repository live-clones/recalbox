################################################################################
#
# pwm-audio-pizero-gpicase
#
################################################################################

PWM_AUDIO_PI_ZERO_VERSION = 66cab631803aef8495616ce1460881c764be38ca
PWM_AUDIO_PI_ZERO_SITE = $(call github,ian57,pwm-audio-pizero-gpicase,$(PWM_AUDIO_PI_ZERO_VERSION))
PWM_AUDIO_PI_ZERO_DEPENDENCIES += rpi-firmware

define PWM_AUDIO_PI_ZERO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/pwm-audio-pizero-gpicase-overlay.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/pwm-audio-pizero-gpicase.dtbo
	$(INSTALL) -D -m 0644 $(@D)/pwm-audio-pizero-gpicase-overlay.dts $(BINARIES_DIR)/rpi-firmware/overlays/pwm-audio-pizero-gpicase.dts
endef

$(eval $(generic-package))
