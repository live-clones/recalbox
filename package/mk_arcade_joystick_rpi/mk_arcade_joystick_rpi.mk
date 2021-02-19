################################################################################
#
# MK_ARCADE_JOYSTICK_RPI
#
################################################################################

MK_ARCADE_JOYSTICK_RPI_VERSION = v0.1.9
MK_ARCADE_JOYSTICK_RPI_SITE = https://gitlab.com/recalbox/mk_arcade_joystick_rpi
MK_ARCADE_JOYSTICK_RPI_SITE_METHOD = git
MK_ARCADE_JOYSTICK_RPI_LICENSE = GPL-2.0
MK_ARCADE_JOYSTICK_RPI_LICENSE_FILES = LICENSE

MK_ARCADE_JOYSTICK_RPI_DEPENDENCIES = linux

define MK_ARCADE_JOYSTICK_RPI_MAKE_HOOK
	cp $(@D)/Makefile.cross $(@D)/Makefile
endef
MK_ARCADE_JOYSTICK_RPI_PRE_BUILD_HOOKS += MK_ARCADE_JOYSTICK_RPI_MAKE_HOOK

$(eval $(kernel-module))
$(eval $(generic-package))
