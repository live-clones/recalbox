################################################################################
#
# VICE
#
################################################################################

LIBRETRO_VICE_VERSION = f509264e85dd18488d85612e70c1417b512345b6
LIBRETRO_VICE_SITE = $(call github,libretro,vice-libretro,$(LIBRETRO_VICE_VERSION))

define LIBRETRO_VICE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=x64
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_x64_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_x64_libretro.so
endef

define LIBRETRO_VICE_BUILD_CMDS
	find $(@D) -name *.so -delete
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=x64sc
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_x64sc_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_x64sc_libretro.so
endef

define LIBRETRO_VICE_BUILD_CMDS
	find $(@D) -name *.so -delete
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=x128
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_x128_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_x128_libretro.so
endef

define LIBRETRO_VICE_BUILD_CMDS
	find $(@D) -name *.so -delete
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=xpet
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_xpet_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_xpet_libretro.so
endef

define LIBRETRO_VICE_BUILD_CMDS
	find $(@D) -name *.so -delete
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=xplus4
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_xplus4_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_xplus4_libretro.so
endef

define LIBRETRO_VICE_BUILD_CMDS
	find $(@D) -name *.so -delete
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=xvic
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_xvic_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_xvic_libretro.so
endef

define LIBRETRO_VICE_PRE_PATCH_FIXUP
	$(SED) 's/\r//g' $(@D)/libretro/libretro-core.c
endef

LIBRETRO_VICE_PRE_PATCH_HOOKS += LIBRETRO_VICE_PRE_PATCH_FIXUP

$(eval $(generic-package))
