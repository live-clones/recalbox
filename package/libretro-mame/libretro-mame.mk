################################################################################
#
# MAME
#
################################################################################

LIBRETRO_MAME_VERSION = ed987ad07964a938351ff3cc1ad42e02ffd2af6d
LIBRETRO_MAME_SITE = $(call github,libretro,mame,$(LIBRETRO_MAME_VERSION))
LIBRETRO_MAME_LICENSE = MAME

LIBRETRO_MAME_OPTS = PTR64=0
# x86_64
ifeq ($(BR2_x86_64),y)
LIBRETRO_MAME_OPTS = PTR64=1
endif

LIBRETRO_MAME_OPTS += platform="unix"
LIBRETRO_MAME_OPTS += LIBRETRO_CPU="$(BR2_ARCH)"
LIBRETRO_MAME_OPTS += LIBRETRO_OS="unix"
LIBRETRO_MAME_OPTS += CONFIG="libretro"
LIBRETRO_MAME_OPTS += OSD="retro"
LIBRETRO_MAME_OPTS += ARCHOPTS="$(COMPILER_COMMONS_BOARD_OPTIONS)"
LIBRETRO_MAME_OPTS += OPTIMIZE=3
LIBRETRO_MAME_OPTS += LTO=0
LIBRETRO_MAME_OPTS += NOWERROR=1
LIBRETRO_MAME_OPTS += PYTHON_EXECUTABLE=python2
LIBRETRO_MAME_OPTS += REGENIE=1
LIBRETRO_MAME_OPTS += VERBOSE=1
LIBRETRO_MAME_OPTS += SUBTARGET=arcade

define LIBRETRO_MAME_BUILD_CMDS
	mkdir -p $(@D)/build/gmake/libretro/obj/x64/libretro/src/osd/retro
	mkdir -p $(@D)/3rdparty/genie/build/gmake.linux/obj/Release/src/host
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_CC)-ar" -C $(@D)/ -f makefile $(LIBRETRO_MAME_OPTS)
endef

define LIBRETRO_MAME_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mamearcade_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame_libretro.so
endef

$(eval $(generic-package))
