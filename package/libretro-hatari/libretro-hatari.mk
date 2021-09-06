################################################################################
#
# HATARI
#
################################################################################

LIBRETRO_HATARI_VERSION = 561c07ee5143d651ee0c04ce31b91ffebc95e299
LIBRETRO_HATARI_SITE = $(call github,libretro,hatari,$(LIBRETRO_HATARI_VERSION))
LIBRETRO_HATARI_LICENSE = GPL-2.0

LIBRETRO_HATARI_DEPENDENCIES = libcapsimage

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64)$(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
LIBRETRO_HATARI_COMPILER_COMMONS_CFLAGS=$(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_HATARI_COMPILER_COMMONS_CXXFLAGS=$(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_HATARI_COMPILER_COMMONS_LDFLAGS=$(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_HATARI_COMPILER_COMMONS_CFLAGS=$(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_HATARI_COMPILER_COMMONS_CXXFLAGS=$(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_HATARI_COMPILER_COMMONS_LDFLAGS=$(COMPILER_COMMONS_LDFLAGS_SO)
endif

LIBRETRO_HATARI_CAPSIMAGE_FLAGS = -DHAVE_CAPSIMAGE=1 -DCAPSIMAGE_VERSION=5

define LIBRETRO_HATARI_BUILD_CMDS
	#$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_HATARI_COMPILER_COMMONS_CFLAGS) $(LIBRETRO_HATARI_CAPSIMAGE_FLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_HATARI_COMPILER_COMMONS_CXXFLAGS) $(LIBRETRO_HATARI_CAPSIMAGE_FLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_HATARI_COMPILER_COMMONS_LDFLAGS) $(LIBRETRO_HATARI_CAPSIMAGE_FLAGS)" \
		ASFLAGS="$(TARGET_ASFLAGS)" PLATFLAGS="$(TARGET_PLATFLAGS)" SHARED="$(TARGET_SHARED)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_HATARI_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/hatari_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/hatari_libretro.so
endef

$(eval $(generic-package))
