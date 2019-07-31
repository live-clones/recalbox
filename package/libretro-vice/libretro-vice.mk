################################################################################
#
# VICE
#
################################################################################

LIBRETRO_VICE_VERSION = f509264e85dd18488d85612e70c1417b512345b6
LIBRETRO_VICE_SITE = $(call github,libretro,vice-libretro,$(LIBRETRO_VICE_VERSION))

LIBRETRO_VICE_SUBEMULATORS = x64 x64sc x128 xpet xplus4 xvic

define LIBRETRO_VICE_BUILD_EMULATOR
	find $(@D) -name *.o -delete; \
	find $(@D) -name *.a -delete; \
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)" EMUTYPE=$(strip $(1));
endef

define LIBRETRO_VICE_BUILD_CMDS
	$(foreach emulator, $(LIBRETRO_VICE_SUBEMULATORS), $(call LIBRETRO_VICE_BUILD_EMULATOR, $(emulator)))
endef

define LIBRETRO_VICE_INSTALL_EMULATOR
	@echo "Installing $(1)..."; \
	$(INSTALL) -D $(@D)/vice_$(strip $(1))_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_$(strip $(1))_libretro.so ;
endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(foreach emulator, $(LIBRETRO_VICE_SUBEMULATORS), $(call LIBRETRO_VICE_INSTALL_EMULATOR, $(emulator)))
endef


define LIBRETRO_VICE_PRE_PATCH_FIXUP
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	$(SED) 's/\r//g' $(@D)/libretro/libretro-core.c
endef

LIBRETRO_VICE_PRE_PATCH_HOOKS += LIBRETRO_VICE_PRE_PATCH_FIXUP

$(eval $(generic-package))
