################################################################################
#
# joycond
#
################################################################################

JOYCOND_VERSION = 2990fe2b156697e7eca6f32be5a2d22ed26f6386
JOYCOND_SITE = $(call github,DanielOgorchock,joycond,$(JOYCOND_VERSION))
JOYCOND_DEPENDENCIES = eudev libevdev hid-nintendo
JOYCOND_LICENSE = GPL-3.0
JOYCOND_LICENSE_FILES = LICENSE

define JOYCOND_INIT_INSTALL
	$(INSTALL) -D -m 0755 $(JOYCOND_PKGDIR)/joycond-init $(TARGET_DIR)/etc/init.d/S17joycond
endef

JOYCOND_POST_INSTALL_TARGET_HOOKS += JOYCOND_INIT_INSTALL

$(eval $(cmake-package))
