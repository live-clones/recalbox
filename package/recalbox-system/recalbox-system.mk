################################################################################
#
# Recalbox System
#
################################################################################

RECALBOX_SYSTEM_SOURCE=

RECALBOX_SYSTEM_VERSION=0.2

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
RECALBOX_SYSTEM_PLATFORM=rpi3
RECALBOX_SYSTEM_RECALBOX_CONF=rpi3
RECALBOX_SYSTEM_FIRMWAREDIR=rpi-firmware
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
RECALBOX_SYSTEM_PLATFORM=odroidxu4
RECALBOX_SYSTEM_RECALBOX_CONF=xu4
RECALBOX_SYSTEM_FIRMWAREDIR=
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
RECALBOX_SYSTEM_PLATFORM=x86
RECALBOX_SYSTEM_RECALBOX_CONF=x86
RECALBOX_SYSTEM_FIRMWAREDIR=
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
RECALBOX_SYSTEM_PLATFORM=x86_64
RECALBOX_SYSTEM_RECALBOX_CONF=x86_64
RECALBOX_SYSTEM_FIRMWAREDIR=
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
RECALBOX_SYSTEM_PLATFORM=rpi2
RECALBOX_SYSTEM_RECALBOX_CONF=rpi2
RECALBOX_SYSTEM_FIRMWAREDIR=rpi-firmware
else
RECALBOX_SYSTEM_PLATFORM=rpi1
RECALBOX_SYSTEM_RECALBOX_CONF=rpi1
RECALBOX_SYSTEM_FIRMWAREDIR=rpi-firmware
endif

define RECALBOX_SYSTEM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/
	echo -n "$(RECALBOX_SYSTEM_PLATFORM)" > $(TARGET_DIR)/recalbox/recalbox.arch
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system
	cp $(RECALBOX_SYSTEM_PKGDIR)/$(RECALBOX_SYSTEM_RECALBOX_CONF)/recalbox.conf $(TARGET_DIR)/recalbox/share_init/system
	cp $(RECALBOX_SYSTEM_PKGDIR)/$(RECALBOX_SYSTEM_RECALBOX_CONF)/recalbox.conf $(TARGET_DIR)/recalbox/share_init/system/recalbox.conf.template
	# recalbox-boot.conf
	$(INSTALL) -D -m 0644 $(RECALBOX_SYSTEM_PKGDIR)/recalbox-boot.conf $(BINARIES_DIR)/$(RECALBOX_SYSTEM_FIRMWAREDIR)/recalbox-boot.conf
endef

$(eval $(generic-package))
