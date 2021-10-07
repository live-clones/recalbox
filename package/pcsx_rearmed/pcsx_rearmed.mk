################################################################################
#
# PCSX_REARMED
#
################################################################################

PCSX_REARMED_VERSION = 7bbabe80066083236adfd653f289de17443a7dd8
PCSX_REARMED_SITE = git://github.com/notaz/pcsx_rearmed.git
PCSX_REARMED_DEPENDENCIES = sdl sdl2 libpng zlib pulseaudio
PCSX_REARMED_GIT_SUBMODULES = YES
PCSX_REARMED_LICENSE = GPL-2.0
PCSX_REARMED_LICENSE_FILES = COPYING

PCSX_REARMED_MAKEFILE_CONFIGURATION  = TARGET=pcsx_rearmed
PCSX_REARMED_MAKEFILE_CONFIGURATION += NO_CONFIG_MAK=yes
PCSX_REARMED_MAKEFILE_CONFIGURATION += ARCH=arm
PCSX_REARMED_MAKEFILE_CONFIGURATION += HAVE_NEON=0
PCSX_REARMED_MAKEFILE_CONFIGURATION += PLATFORM=generic
PCSX_REARMED_MAKEFILE_CONFIGURATION += BUILTIN_GPU=peops
PCSX_REARMED_MAKEFILE_CONFIGURATION += SOUND_DRIVERS=sdl
PCSX_REARMED_MAKEFILE_CONFIGURATION += PLUGIN=
PCSX_REARMED_MAKEFILE_CONFIGURATION += USE_DYNAREC=1

PCSX_REARMED_SDL_INCLUDE = -I$(STAGING_DIR)/usr/include/SDL
PCSX_REARMED_SDL_LIBRARY = -L$(STAGING_DIR)/usr/lib
PCSX_REARMED_ALL_LIBRARIES = $(PCSX_REARMED_SDL_LIBRARY) -ldl -lpthread -lSDL -lpng -lz

PCSX_REARMED_CONF_OPTS += --sound-drivers=pulseaudio
PCSX_REARMED_CONF_ENV += PATH=$(STAGING_DIR)/usr/bin:$$PATH

ifeq ($(BR2_arm1176jzf_s),y)
PCSX_REARMED_CONF_ENV += CFLAGS=-march=armv6
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
PCSX_REARMED_CONF_OPTS += --enable-neon
else
PCSX_REARMED_CONF_OPTS += --disable-neon
endif

#define PCSX_REARMED_BUILD_CMDS
#	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
#	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO) $(PCSX_REARMED_SDL_INCLUDE)" \
#		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO) $(PCSX_REARMED_SDL_INCLUDE)" \
#		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO) $(PCSX_REARMED_ALL_LIBRARIES)" \
#		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D) -f Makefile $(PCSX_REARMED_MAKEFILE_CONFIGURATION)
#endef

define PCSX_REARMED_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/pcsx \
		$(TARGET_DIR)/usr/bin/pcsx_rearmed
	$(INSTALL) -D $(@D)/plugins/dfxvideo/gpu_peops.so $(TARGET_DIR)/recalbox/share_init/system/configs/pcsx/plugins/gpu_peops.so
	$(INSTALL) -D $(@D)/plugins/gpu_unai/gpu_unai.so $(TARGET_DIR)/recalbox/share_init/system/configs/pcsx/plugins/gpu_unai.so
	$(INSTALL) -D $(@D)/plugins/spunull/spunull.so $(TARGET_DIR)/recalbox/share_init/system/configs/pcsx/plugins/spunull.so
endef

define PCSX_REARMED_PRE_PATCH_FIXUP
	$(SED) 's/\r//g' $(@D)/libpcsxcore/plugins.c
	$(SED) 's/\r//g' $(@D)/libpcsxcore/plugins.h
endef

PCSX_REARMED_PRE_PATCH_HOOKS += PCSX_REARMED_PRE_PATCH_FIXUP

$(eval $(autotools-package))
