################################################################################
#
# REICAST (NEW VERSION)
#
################################################################################

REICAST_NEW_VERSION = ad61f95dd6b4c6218846a25de6194550311a5d28
REICAST_NEW_SITE = $(call github,reicast,reicast-emulator,$(REICAST_NEW_VERSION))
REICAST_NEW_DEPENDENCIES = sdl2 libpng

define REICAST_NEW_UPDATE_INCLUDES
	sed -i "s+/opt/vc+$(STAGING_DIR)/usr+g" $(@D)/shell/linux/Makefile
	sed -i "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/shell/linux/Makefile
endef
REICAST_NEW_PRE_CONFIGURE_HOOKS += REICAST_NEW_UPDATE_INCLUDES

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
REICAST_NEW_RECALBOX_SYSTEM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
REICAST_NEW_RECALBOX_SYSTEM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
REICAST_NEW_RECALBOX_SYSTEM=odroidxu3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
REICAST_NEW_RECALBOX_SYSTEM=x86
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
REICAST_NEW_RECALBOX_SYSTEM=x64
endif

# Sadly the NEON optimizations in the PNG library don't work yet, so disable them
define REICAST_NEW_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) \
		CPP="$(TARGET_CPP)" \
		CXX="$(TARGET_CXX) -D_GLIBCXX_USE_CXX11_ABI=0" \
		CC="$(TARGET_CC) -DPNG_ARM_NEON_OPT=0" \
		AS="$(TARGET_CC)" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D)/shell/linux -f Makefile platform=$(REICAST_NEW_RECALBOX_SYSTEM)
endef

define REICAST_NEW_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/shell/linux/reicast.elf $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
