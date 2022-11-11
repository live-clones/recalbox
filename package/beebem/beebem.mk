################################################################################
#
# Beebem
#
################################################################################

BEEBEM_VERSION = 5e81412d73bcd8e1b4b5944e8ad4787346252a89
BEEBEM_SITE = https://gitlab-ci-token-beebem:$(GITLAB_TOKEN_BEEBEM)@gitlab.com/bkg2k/beebem-linux.git
BEEBEM_SITE_METHOD = git
BEEBEM_LICENSE = GPL
BEEBEM_DEPENDENCIES = sdl2 sdl2_ttf zlib libfreeimage libzip libcurl procps-ng

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
BEEBEM_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
BEEBEM_DEPENDENCIES += libgles
endif

BEEBEM_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
BEEBEM_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
BEEBEM_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
BEEBEM_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
BEEBEM_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
BEEBEM_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_EXE)"
BEEBEM_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_EXE)"
BEEBEM_CONF_OPTS += -DCMAKE_EXE_LINKER_FLAGS="$(COMPILER_COMMONS_LDFLAGS_EXE)"

ifneq ($(GITLAB_TOKEN_BEEBEM),)
$(eval $(cmake-package))
endif
