################################################################################
#
# SCUMMVM
#
################################################################################

LIBRETRO_SCUMMVM_VERSION = 440d5d37ed8b4e0f7d0e312b74cae1bf3506a8fd
LIBRETRO_SCUMMVM_SITE = $(call github,libretro,scummvm,$(LIBRETRO_SCUMMVM_VERSION))
LIBRETRO_SCUMMVM_LICENSE = GPL-2.0
LIBRETRO_SCUMMVM_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64)$(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
LIBRETRO_SCUMMVM_LTO_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_SCUMMVM_LTO_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_SCUMMVM_LTO_LDFLAGS = $(COMPILER_COMMONS_LSFLAGS_NOLTO)
else
LIBRETRO_SCUMMVM_LTO_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_SCUMMVM_LTO_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_SCUMMVM_LTO_LDFLAGS = $(COMPILER_COMMONS_LSFLAGS_SO)
endif

define LIBRETRO_SCUMMVM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/backends/platform/libretro/build/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_SCUMMVM_LTO_CFLAGS) -ffat-lto-objects" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_SCUMMVM_LTO_CXXFLAGS) -ffat-lto-objects" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_SCUMMVM_LTO_LDFLAGS) -ffat-lto-objects -shared -Wl,--no-undefined" \
		$(MAKE) TOOLSET="$(TARGET_CROSS)" -C $(@D)/backends/platform/libretro/build platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_SCUMMVM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/libretro-scummvm
	unzip -o $(@D)/backends/platform/libretro/aux-data/scummvm.zip -d $(TARGET_DIR)/usr/share/libretro-scummvm
	$(INSTALL) -D $(@D)/backends/platform/libretro/build/scummvm_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/scummvm_libretro.so
endef

$(eval $(generic-package))
