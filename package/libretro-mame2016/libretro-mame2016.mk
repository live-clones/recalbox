################################################################################
#
# MAME2016
#
################################################################################

LIBRETRO_MAME2016_VERSION = 02987af9b81a9c3294af8fb9d5a34f9826a2cf4d
LIBRETRO_MAME2016_SITE = $(call github,libretro,mame2016-libretro,$(LIBRETRO_MAME2016_VERSION))
LIBRETRO_MAME2016_LICENSE = MAME
LIBRETRO_MAME2016_NON_COMMERCIAL = y

# x86_64
ifeq ($(BR2_x86_64),y)
LIBRETRO_MAME2016_OPTS = PTR64=1
else
LIBRETRO_MAME2016_OPTS = PTR64=0
endif

LIBRETRO_MAME2016_OPTS += platform="unix"
LIBRETRO_MAME2016_OPTS += LIBRETRO_CPU="$(BR2_ARCH)"
LIBRETRO_MAME2016_OPTS += LIBRETRO_OS="unix"
LIBRETRO_MAME2016_OPTS += CONFIG="libretro"
LIBRETRO_MAME2016_OPTS += OSD="retro"
LIBRETRO_MAME2016_OPTS += ARCHOPTS="$(COMPILER_COMMONS_BOARD_OPTIONS)"
LIBRETRO_MAME2016_OPTS += OPTIMIZE=3
LIBRETRO_MAME2016_OPTS += LTO=1
LIBRETRO_MAME2016_OPTS += NOWERROR=1
LIBRETRO_MAME2016_OPTS += PYTHON_EXECUTABLE=python2
LIBRETRO_MAME2016_OPTS += REGENIE=1
LIBRETRO_MAME2016_OPTS += VERBOSE=1
LIBRETRO_MAME2016_OPTS += SUBTARGET=arcade

define LIBRETRO_MAME2016_BUILD_CMDS
	mkdir -p $(@D)/build/gmake/libretro/obj/x64/libretro/src/osd/retro
	mkdir -p $(@D)/3rdparty/genie/build/gmake.linux/obj/Release/src/host
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_CC)-ar" -C $(@D)/ -f makefile $(LIBRETRO_MAME2016_OPTS)
endef

define LIBRETRO_MAME2016_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mamearcade2016_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2016_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2016/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2016
endef

$(eval $(generic-package))
