################################################################################
#
# rtl8812au
#
################################################################################

RTL8812AU_VERSION = 37e27f9165300c89607144b646545fac576ec510
RTL8812AU_SITE = $(call github,aircrack-ng,rtl8812au,$(RTL8812AU_VERSION))
RTL8812AU_LICENSE = GPL-2.0
RTL8812AU_LICENSE_FILES = LICENSE

RTL8812AU_MODULE_MAKE_OPTS = \
	KSRC="$(LINUX_DIR)" \
	KVER="$(LINUX_VERSION_PROBED)" \
	INSTALL_MOD_PATH="$(TARGET_DIR)" \
	-C $(@D)

$(eval $(kernel-module))
$(eval $(generic-package))
