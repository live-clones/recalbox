################################################################################
#
# PCSX_REARMED
#
################################################################################

PCSX_REARMED_VERSION = 7bbabe80066083236adfd653f289de17443a7dd8
PCSX_REARMED_SITE = git://github.com/notaz/pcsx_rearmed.git
PCSX_REARMED_DEPENDENCIES = sdl libpng zlib pulseaudio libgles
PCSX_REARMED_GIT_SUBMODULES = YES
PCSX_REARMED_LICENSE = GPL-2.0
PCSX_REARMED_LICENSE_FILES = COPYING

PCSX_REARMED_CONF_ENV += \
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	CPPFLAGS="$(TARGET_CPPFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
	ROOT_DIR="$(STAGING_DIR)"
PCSX_REARMED_CONF_ENV += PATH=$(STAGING_DIR)/usr/bin:$$PATH

PCSX_REARMED_CONF_OPTS += --sound-drivers=pulseaudio

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
PCSX_REARMED_CONF_OPTS += --enable-neon
else
PCSX_REARMED_CONF_OPTS += --disable-neon
endif

define PCSX_REARMED_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/pcsx \
		$(TARGET_DIR)/usr/bin/pcsx_rearmed
	$(INSTALL) -D $(@D)/plugins/gpu-gles/gpu_gles.so  $(TARGET_DIR)/usr/lib/pcsx_rearmed/plugins/gpu_gles.so
	$(INSTALL) -D $(@D)/plugins/dfxvideo/gpu_peops.so $(TARGET_DIR)/usr/lib/pcsx_rearmed/plugins/gpu_peops.so
	$(INSTALL) -D $(@D)/plugins/gpu_unai/gpu_unai.so  $(TARGET_DIR)/usr/lib/pcsx_rearmed/plugins/gpu_unai.so
	$(INSTALL) -D $(@D)/plugins/spunull/spunull.so    $(TARGET_DIR)/usr/lib/pcsx_rearmed/plugins/spunull.so
endef

$(eval $(autotools-package))
