################################################################################
#
# TI99SIM - TI-99 Emulator
#
################################################################################

TI99SIM_VERSION = v0.16.0
TI99SIM_SITE = $(call gitlab,recalbox,packages/standalone/ti99sim,$(TI99SIM_VERSION))
TI99SIM_DEPENDENCIES = sdl2
TI99SIM_LICENSE = GPL-2.0
TI99SIM_LICENSE_FILES = doc/COPYING

define TI99SIM_PATCH_MAKEFILES
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|--std=c++2a|-std=c++20|g" $(@D)/rules.mak
	$(SED) "s|sdl2-config|$(STAGING_DIR)/usr/bin/sdl2-config|g" $(@D)/src/sdl/Makefile
	$(SED) "s|sdl2-config|$(STAGING_DIR)/usr/bin/sdl2-config|g" $(@D)/src/util/Makefile
endef

TI99SIM_POST_PATCH_HOOKS += TI99SIM_PATCH_MAKEFILES

define TI99SIM_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile OS=OS_LINUX
endef

define TI99SIM_INSTALL_TARGET_CMDS
	$(TARGET_STRIP) -s $(@D)/bin/*
	mkdir -p $(TARGET_DIR)/usr/bin/ti99sim
	$(INSTALL) -D $(@D)/bin/* $(TARGET_DIR)/usr/bin/ti99sim
endef

$(eval $(generic-package))
