################################################################################
#
# MK_ARCADE_JOYSTICK_RPI
#
################################################################################
ifeq ($(BR2_PACKAGE_MK_ARCADE_JOYSTICK_RPI_HOTKEY),y)
	MK_ARCADE_JOYSTICK_RPI_VERSION = febcb904df9a5c2a40b905c60d6926b9266377b6
	#v0.1.6-hotkey-custom-gpio
else
	MK_ARCADE_JOYSTICK_RPI_VERSION = v0.1.5
endif	
#MK_ARCADE_JOYSTICK_RPI_SITE = $(call gitlab,recalbox,mk_arcade_joystick_rpi,$(MK_ARCADE_JOYSTICK_RPI_VERSION))
MK_ARCADE_JOYSTICK_RPI_SITE = https://gitlab.com/recalbox/mk_arcade_joystick_rpi
MK_ARCADE_JOYSTICK_RPI_SITE_METHOD = git

MK_ARCADE_JOYSTICK_RPI_DEPENDENCIES = linux


# Needed beacause can't pass cflags to cc
define MK_ARCADE_JOYSTICK_RPI_RPI2_HOOK
        $(SED) "s/#define PERI_BASE        0x20000000/#define PERI_BASE        0x3F000000/g" $(@D)/mk_arcade_joystick_rpi.c
endef

ifeq ($(BR2_cortex_a7),y)
        MK_ARCADE_JOYSTICK_RPI_PRE_CONFIGURE_HOOKS += MK_ARCADE_JOYSTICK_RPI_RPI2_HOOK
endif
ifeq ($(BR2_cortex_a53),y)
        MK_ARCADE_JOYSTICK_RPI_PRE_CONFIGURE_HOOKS += MK_ARCADE_JOYSTICK_RPI_RPI2_HOOK
endif

define MK_ARCADE_JOYSTICK_RPI_MAKE_HOOK
	cp $(@D)/Makefile.cross $(@D)/Makefile
endef
MK_ARCADE_JOYSTICK_RPI_PRE_BUILD_HOOKS += MK_ARCADE_JOYSTICK_RPI_MAKE_HOOK

define MK_ARCADE_JOYSTICK_RPI_BUILD_CMDS
        $(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define MK_ARCADE_JOYSTICK_RPI_INSTALL_TARGET_CMDS
        $(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
endef

$(eval $(generic-package))
