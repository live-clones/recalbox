################################################################################
#
# joycond
#
################################################################################

JOYCOND_VERSION = 5b590ecc9bca181d8bc21377e752126bc9180319
JOYCOND_SITE = $(call github,DanielOgorchock,joycond,$(JOYCOND_VERSION))
JOYCOND_DEPENDENCIES = eudev libevdev hid-nintendo
JOYCOND_LICENSE = GPL-3.0
JOYCOND_LICENSE_FILES = LICENSE

define JOYCOND_INIT_INSTALL
	$(INSTALL) -D -m 0755 $(JOYCOND_PKGDIR)/joycond-init $(TARGET_DIR)/etc/init.d/S17joycond
endef

JOYCOND_POST_INSTALL_TARGET_HOOKS += JOYCOND_INIT_INSTALL

$(eval $(cmake-package))
