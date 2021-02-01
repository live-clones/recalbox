################################################################################
#
# pwm-audio-pizero-gpicase
#
################################################################################

PWM_AUDIO_PIZERO_GPICASE_VERSION = 4fbcbce5643d243784bd71d8215ac58476c8dacd
PWM_AUDIO_PIZERO_GPICASE_SITE = $(call github,ian57,pwm-audio-pizero-gpicase,$(PWM_AUDIO_PIZERO_GPICASE_VERSION))
PWM_AUDIO_PIZERO_GPICASE_LICENSE = GPL-2.0
PWM_AUDIO_PIZERO_GPICASE_LICENSE_FILES = LICENSE

PWM_AUDIO_PIZERO_GPICASE_DEPENDENCIES += rpi-firmware

define PWM_AUDIO_PIZERO_GPICASE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/pwm-audio-pizero-gpicase.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/pwm-audio-pizero-gpicase.dtbo
	$(INSTALL) -D -m 0644 $(@D)/pwm-audio-pizero-gpicase.dts $(BINARIES_DIR)/rpi-firmware/overlays/pwm-audio-pizero-gpicase.dts
endef

$(eval $(generic-package))
