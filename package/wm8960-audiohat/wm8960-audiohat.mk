################################################################################
#
# WM8960_AUDIOHAT
#
################################################################################

#kernel 5.4
#WM8960_AUDIOHAT_VERSION = cd5d2e01a80cce929ea64715ed73d1b91cd5ba50
#kernel 4.18 for pi1/2/3 actually
#previous release on github
WM8960_AUDIOHAT_VERSION = a9aa1d0a10356fbfb56fcd8a63c7435ac80eb173
ifeq ($(BR2_cortex_a72),y) #rpi4 last release on github  
	WM8960_AUDIOHAT_VERSION = cd5d2e01a80cce929ea64715ed73d1b91cd5ba50
endif


WM8960_AUDIOHAT_SITE = $(call github,waveshare,WM8960-Audio-HAT,$(WM8960_AUDIOHAT_VERSION))
WM8960_AUDIOHAT_DEPENDENCIES = linux

define WM8960_AUDIOHAT_MAKE_HOOK
	cp $(WM8960_AUDIOHAT_PKGDIR)/Makefile.cross $(@D)/Makefile
endef
WM8960_AUDIOHAT_PRE_BUILD_HOOKS += WM8960_AUDIOHAT_MAKE_HOOK

define WM8960_AUDIOHAT_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define WM8960_AUDIOHAT_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
	$(INSTALL) -D -m 0644 $(@D)/wm8960-soundcard.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/wm8960-soundcard.dtbo
	$(INSTALL) -D -m 0644 $(@D)/wm8960-soundcard.dts $(BINARIES_DIR)/rpi-firmware/overlays/wm8960-soundcard.dts
	#$(INSTALL) -D -m 0644 $(@D)/wm8960-soundcard $(TARGET_DIR)/usr/bin
endef

$(eval $(kernel-module))
$(eval $(generic-package))
