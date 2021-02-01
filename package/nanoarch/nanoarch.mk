################################################################################
#
# nanoarch
#
################################################################################

NANOARCH_VERSION = 396ca3a41ee7c61d76e39fa9a98b9dde7116af00
NANOARCH_SITE = $(call github,heuripedes,nanoarch,$(NANOARCH_VERSION))
NANOARCH_LICENSE = BSD-3-Clause
NANOARCH_LICENSE_FILES = LICENSE

define NANOARCH_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" -C $(@D)
endef

define NANOARCH_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/nanoarch \
		$(TARGET_DIR)/usr/bin/nanoarch
endef

$(eval $(generic-package))
