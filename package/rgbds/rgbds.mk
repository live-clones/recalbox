################################################################################
#
# RGBDS
#
################################################################################

RGBDS_VERSION = v0.3.8
RGBDS_SITE = $(call github,rednex,rgbds,$(RGBDS_VERSION))
RGBDS_LICENSE = MIT
RGBDS_LICENSE_FILES = LICENSE
RGBDS_HOST_DEPENDENCIES = host-bison host-flex host-pkgconf host-libpng

# Workaround for hand-written Makefile that has no way to specify actual
# LDFLAGS. It happens though that CFLAGS are also used at link time in the
# Makefile, so we can hijack them. LDFLAGS will be ignored when doing
# compilation.
HOST_RGBDS_CFLAGS = $(HOST_CFLAGS) $(HOST_LDFLAGS)

define HOST_RGBDS_BUILD_CMDS
	$(MAKE) $(HOST_CONFIGURE_OPTS) CFLAGS="$(HOST_RGBDS_CFLAGS)" \
		YACC="$(HOST_DIR)/bin/bison" \
		LEX="$(HOST_DIR)/bin/flex" \
		-C $(@D)
endef

define HOST_RGBDS_INSTALL_CMDS
	$(MAKE) $(HOST_CONFIGURE_OPTS) CFLAGS="$(HOST_RGBDS_CFLAGS)" \
		-C $(@D) PREFIX=$(HOST_DIR) \
		install
endef

$(eval $(host-generic-package))
