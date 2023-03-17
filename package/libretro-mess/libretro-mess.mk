################################################################################
#
# MESS
#
################################################################################

LIBRETRO_MESS_VERSION = 78361d7b213c4d011ebfa73321f659cb5400fd22
LIBRETRO_MESS_SITE = $(call github,libretro,mame,$(LIBRETRO_MESS_VERSION))
LIBRETRO_MESS_LICENSE = MESS
LIBRETRO_MESS_DEPENDENCIES = alsa-lib

ifeq ($(BR2_x86_64),y)
LIBRETRO_MESS_OPTS += PTR64=1
LIBRETRO_MESS_OPTS += LIBRETRO_CPU=x86_64
LIBRETRO_MESS_OPTS += PLATFORM=x86_64
LIBRETRO_MESS_OPTS += ARCHITECTURE=""
LIBRETRO_MESS_ARCHOPTS += -DPTR64=1
endif

ifeq ($(BR2_i386),y)
LIBRETRO_MESS_OPTS += PTR64=0
LIBRETRO_MESS_OPTS += LIBRETRO_CPU=x86
LIBRETRO_MESS_OPTS += PLATFORM=x86
LIBRETRO_MESS_OPTS += ARCHITECTURE=""
endif

ifeq ($(BR2_arm),y)
LIBRETRO_MESS_OPTS += PTR64=0
LIBRETRO_MESS_OPTS += LIBRETRO_CPU=arm
LIBRETRO_MESS_OPTS += PLATFORM=arm
LIBRETRO_MESS_OPTS += ARCHITECTURE=""
LIBRETRO_MESS_OPTS += NOASM=1
LIBRETRO_MESS_ARCHOPTS += -D__arm__
endif

LIBRETRO_MESS_OPTS += ARCHOPTS="$(COMPILER_COMMONS_BOARD_OPTIONS) $(LIBRETRO_MESS_ARCHOPTS)"
LIBRETRO_MESS_OPTS += CONFIG=libretro
LIBRETRO_MESS_OPTS += DEBUG=0
LIBRETRO_MESS_OPTS += DISTRO=debian-stable
LIBRETRO_MESS_OPTS += LIBRETRO_OS=unix
LIBRETRO_MESS_OPTS += NOWERROR=1
LIBRETRO_MESS_OPTS += OPENMP=1
LIBRETRO_MESS_OPTS += OSD=retro
LIBRETRO_MESS_OPTS += PROJECT=""
LIBRETRO_MESS_OPTS += PYTHON_EXECUTABLE=python2
LIBRETRO_MESS_OPTS += REGENIE=0
LIBRETRO_MESS_OPTS += RETRO=1
LIBRETRO_MESS_OPTS += SUBTARGET=mess
LIBRETRO_MESS_OPTS += TARGET=mame
LIBRETRO_MESS_OPTS += TARGETOS=linux
LIBRETRO_MESS_OPTS += VERBOSE=0
LIBRETRO_MESS_OPTS += OPTIMIZE=3

LIBRETRO_MESS_OPTS += CROSS_BUILD=1
LIBRETRO_MESS_OPTS += OVERRIDE_CXX="$(TARGET_CXX)" OVERRIDE_CC="$(TARGET_CC)" OVERRIDE_LD="$(TARGET_LD)"
LIBRETRO_MESS_OPTS += RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)"
# Select driver {System} to build
#LIBRETRO_MESS_OPTS += SOURCES="src/mame/drivers/aquarius.cpp,src/mame/drivers/arcadia.cpp,src/mame/drivers/cdi.cpp,src/mame/drivers/cgenie.cpp,src/mame/drivers/coco3.cpp,src/mame/drivers/einstein.cpp,src/mame/drivers/gp32.cpp,src/mame/drivers/hh_sm510.cpp,src/mame/drivers/mc10.cpp,src/mame/drivers/pockstat.cpp,src/mame/drivers/sega_beena.cpp,src/mame/drivers/segapico.cpp,src/mame/drivers/vsmile.cpp,src/mame/drivers/vsmileb.cpp,src/mame/drivers/vsmilepro.cpp"
LIBRETRO_MESS_SOURCES += "src/mame/drivers/aquarius.cpp,src/mame/drivers/arcadia.cpp,src/mame/drivers/cdi.cpp,src/mame/drivers/cgenie.cpp,src/mame/drivers/coco3.cpp,src/mame/drivers/einstein.cpp,src/mame/drivers/hh_sm510.cpp,src/mame/drivers/mc10.cpp,src/mame/drivers/pockstat.cpp,src/mame/drivers/sega_beena.cpp,src/mame/drivers/segapico.cpp,src/mame/drivers/vsmile.cpp,src/mame/drivers/vsmileb.cpp,src/mame/drivers/vsmilepro.cpp"
LIBRETRO_MESS_OPTS += SOURCES="${LIBRETRO_MESS_SOURCES}"

define LIBRETRO_MESS_BUILD_CMDS
	mkdir -p $(@D)/build/gmake/libretro/obj/x64/libretro/src/osd/retro
	mkdir -p $(@D)/3rdparty/genie/build/gmake.linux/obj/Release/src/host
	$(MAKE) -C $(@D)/ -f makefile $(LIBRETRO_MESS_OPTS)
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
