################################################################################
#
# MESS
#
################################################################################

LIBRETRO_MESS_VERSION = aedbec9df0d0a4e41e38b744a56826fa7898c417
LIBRETRO_MESS_SITE = $(call github,libretro,mame,$(LIBRETRO_MESS_VERSION))
LIBRETRO_MESS_LICENSE = MESS

LIBRETRO_MESS_OPTS = PTR64=0
# x86_64
ifeq ($(BR2_x86_64),y)
LIBRETRO_MESS_OPTS = PTR64=1
endif

LIBRETRO_MESS_OPTS += platform="unix"
LIBRETRO_MESS_OPTS += LIBRETRO_CPU="$(BR2_ARCH)"
LIBRETRO_MESS_OPTS += LIBRETRO_OS="unix"
LIBRETRO_MESS_OPTS += CONFIG="libretro"
LIBRETRO_MESS_OPTS += OSD="retro"
LIBRETRO_MESS_OPTS += ARCHOPTS="$(COMPILER_COMMONS_BOARD_OPTIONS)"
LIBRETRO_MESS_OPTS += OPTIMIZE=3
LIBRETRO_MESS_OPTS += LTO=0
LIBRETRO_MESS_OPTS += NOWERROR=1
LIBRETRO_MESS_OPTS += PYTHON_EXECUTABLE=python2
LIBRETRO_MESS_OPTS += REGENIE=1
LIBRETRO_MESS_OPTS += VERBOSE=1
LIBRETRO_MESS_OPTS += SUBTARGET=mess

define LIBRETRO_MESS_BUILD_CMDS
	mkdir -p $(@D)/build/gmake/libretro/obj/x64/libretro/src/osd/retro
	mkdir -p $(@D)/3rdparty/genie/build/gmake.linux/obj/Release/src/host
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_CC)-ar" -C $(@D)/ -f makefile $(LIBRETRO_MESS_OPTS)
endef

define LIBRETRO_MESS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mess_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mess_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/mame
	cp -r $(@D)/artwork \
		$(TARGET_DIR)/recalbox/share_init/bios/mame/artwork
	cp -r $(@D)/hash \
		$(TARGET_DIR)/recalbox/share_init/bios/mame/hash
	cp -r $(@D)/samples \
		$(TARGET_DIR)/recalbox/share_init/bios/mame/samples
endef

$(eval $(generic-package))
