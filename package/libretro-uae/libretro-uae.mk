################################################################################
#
# UAE
#
################################################################################

LIBRETRO_UAE_VERSION = 067de39c8898fcc937f45353c7a100c839663c6d
LIBRETRO_UAE_SITE = $(call github,libretro,libretro-uae,$(LIBRETRO_UAE_VERSION))

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
UAEPLATFORM=rpi
else
UAEPLATFORM=unix
ifeq ($(BR2_ARM_CPU_HAS_ARM),y)
UAEPLATFLAGS=-DARM -marm
endif
ifeq ($(BR2_aarch64),y)
UAEPLATFLAGS=-DAARCH64
endif
endif

ifeq ($(BR2_x86_64),y)
# x64 does not support LTO
LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif

define LIBRETRO_UAE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|^PLATFLAGS :=|PLATFLAGS := $(UAEPLATFLAGS)|g" $(@D)/Makefile
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/retrodep/machdep/maccess.h
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/retrodep/machdep/m68kops.h
	# Rename bios to match amiberry
	$(SED) 's/"kick34005.A500"/"kick34005.A500.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick37175.A500"/"kick37175.A500.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick40063.A600"/"kick40063.A600.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick40068.A1200"/"kick40068.A1200.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick40068.A4000"/"kick40068.A4000.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick34005.CDTV"/"kick34005.CDTV.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick40060.CD32"/"kick40060.CD32.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/"kick40060.CD32.ext"/"kick40060.CD32.ext.rom"/g' $(@D)/libretro/libretro-glue.h
	$(SED) 's/kick33180.A500/kick33180.A500.rom/g' $(@D)/whdload/WHDLoad_files/S/Startup-Sequence
	$(SED) 's/kick34005.A500/kick34005.A500.rom/g' $(@D)/whdload/WHDLoad_files/S/Startup-Sequence
	$(SED) 's/kick40063.A600/kick40063.A600.rom/g' $(@D)/whdload/WHDLoad_files/S/Startup-Sequence
	$(SED) 's/kick40068.A1200/kick40068.A1200.rom/g' $(@D)/whdload/WHDLoad_files/S/Startup-Sequence
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(UAEPLATFORM)"
endef

define LIBRETRO_UAE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/puae_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/puae_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/uae_data
endef

$(eval $(generic-package))
