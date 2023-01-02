################################################################################
#
# SDL1.2 Compatibility library
#
################################################################################

SDL_VERSION = release-1.2.60
SDL_SITE = https://github.com/libsdl-org/sdl12-compat.git
SDL_SITE_METHOD = git
SDL_DEPENDENCIES = sdl2
SDL_LICENSE = custom
SDL_LICENSE_FILES = LICENSE.txt
SDL_INSTALL_STAGING = YES

SDL_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SDL_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
SDL_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SDL_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
SDL_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
SDL_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
SDL_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
SDL_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
SDL_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_SO)"
SDL_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_SO)"
SDL_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_SO)"
# No tests
SDL_CONF_OPTS += -DSDL12TESTS=OFF
# Route install into staging to get good path info sdl-config
SDL_CONF_OPTS += -DSDL12_COMPAT_LIBDIR="$(STAGING_DIR)/usr/lib"
SDL_CONF_OPTS += -DSDL12_COMPAT_INCLUDEDIR="$(STAGING_DIR)/usr/include"

define SDL_INSTALL_TARGET_CMDS
	cp --no-dereference --preserve=links -u $(@D)/libSDL* $(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))
