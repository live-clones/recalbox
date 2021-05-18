################################################################################
#
# rtl8821cu
#
################################################################################

RTL8821CU_VERSION = 9c1b069f46ed565f0e4a3a54a76493b6ab00efad
RTL8821CU_SITE = $(call github,morrownr,8821cu,$(RTL8821CU_VERSION))
RTL8821CU_LICENSE = GPL-2.0
RTL8821CU_LICENSE_FILES = LICENSE

RTL8821CU_MODULE_MAKE_OPTS = \
	KSRC="$(LINUX_DIR)" \
	KVER="$(LINUX_VERSION_PROBED)" \
	INSTALL_MOD_PATH="$(TARGET_DIR)" \
	-C $(@D)

$(eval $(kernel-module))
$(eval $(generic-package))
