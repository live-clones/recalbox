################################################################################
#
# rgb-pi
#
################################################################################

RGB_PI_VERSION = 637a3917ed8b3cf0c19314d9ee4ce5242837c5ae
#RGB_PI_SITE = git://github.com/mortaca/RGB-Pi
RGB_PI_SITE = $(call github,mortaca,rgb-pi,$(RGB_PI_VERSION))
RGB_PI_DEPENDENCIES += rpi-firmware

define RGB_PI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/overlays/rgb-pi.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/
endef

$(eval $(generic-package))
