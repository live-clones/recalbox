################################################################################
#
# REICAST
#
################################################################################

REICAST_VERSION = 0bd6ea371e0293f3c8e02b5ac1344c89b1e60bee
REICAST_SITE = $(call github,reicast,reicast-emulator,$(REICAST_VERSION))
REICAST_DEPENDENCIES = sdl2 libpng

define REICAST_UPDATE_INCLUDES
	sed -i "s+/opt/vc+$(STAGING_DIR)/usr+g" $(@D)/reicast/linux/Makefile
	sed -i "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/reicast/linux/Makefile
endef
REICAST_PRE_CONFIGURE_HOOKS += REICAST_UPDATE_INCLUDES

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
REICAST_RECALBOX_SYSTEM += rpi4-mesa
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
REICAST_RECALBOX_SYSTEM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
REICAST_RECALBOX_SYSTEM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
REICAST_RECALBOX_SYSTEM=odroidxu3
REICAST_RECALBOX_EXTRA_ARGS += ARM32_REC=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86),y)
REICAST_RECALBOX_SYSTEM=x86
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
REICAST_RECALBOX_SYSTEM=x64
endif

ifeq ($(BR2_PACKAGE_SDL2_KMSDRM),y)
REICAST_RECALBOX_EXTRA_ARGS += USE_SDL=1
REICAST_RECALBOX_EXTRA_ARGS += SUPPORT_SDL=1
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
REICAST_RECALBOX_EXTRA_ARGS += USE_GLES=1
endif

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
REICAST_RECALBOX_EXTRA_ARGS += SUPPORT_EGL=1
endif

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
REICAST_RECALBOX_EXTRA_ARGS += USE_ALSA=1
endif

ifeq ($(BR2_PACKAGE_PULSEAUDIO),y)
REICAST_RECALBOX_EXTRA_ARGS += USE_PULSEAUDIO=1
endif

REICAST_RECALBOX_EXTRA_ARGS += PLATFORM_LINUX=1

# Sadly the NEON optimizations in the PNG library doesn't work yet, so disable them
define REICAST_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/reicast/linux/Makefile
	$(SED) "s|CFLAGS :=|CFLAGS := $(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)|g" $(@D)/reicast/linux/Makefile
	$(SED) "s|CXXFLAGS :=|CXXFLAGS := $(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)|g" $(@D)/reicast/linux/Makefile
	$(SED) "s|LDFLAGS :=|LDFLAGS := $(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)|g" $(@D)/reicast/linux/Makefile
	$(TARGET_CONFIGURE_OPTS) $(MAKE) \
		CPP="$(TARGET_CPP)" \
		CXX="$(TARGET_CXX) -D_GLIBCXX_USE_CXX11_ABI=0" \
		CC="$(TARGET_CC) -DPNG_ARM_NEON_OPT=0" \
		AS="$(TARGET_CC)" \
		AR="$(TARGET_CC)-ar" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D)/reicast/linux -f Makefile platform=$(REICAST_RECALBOX_SYSTEM) $(REICAST_RECALBOX_EXTRA_ARGS)
endef

define REICAST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/reicast/linux/reicast.elf $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
