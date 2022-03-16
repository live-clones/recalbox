################################################################################
#
# rtl8821cu
#
################################################################################

RTL8821CU_VERSION = 4bdd7c8668562e43564cd5d786055633e591ad4d
RTL8821CU_SITE = $(call github,morrownr,8821cu-20210118,$(RTL8821CU_VERSION))
RTL8821CU_LICENSE = GPL-2.0
RTL8821CU_LICENSE_FILES = LICENSE

RTL8821CU_MODULE_MAKE_OPTS = \
	KSRC="$(LINUX_DIR)" \
	KVER="$(LINUX_VERSION_PROBED)" \
	INSTALL_MOD_PATH="$(TARGET_DIR)" \
	-C $(@D)

$(eval $(kernel-module))
$(eval $(generic-package))
