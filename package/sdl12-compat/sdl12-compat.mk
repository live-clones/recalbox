################################################################################
#
# SDL1.2 Compatibility library
#
################################################################################

SDL12_COMPAT_VERSION = release-1.2.60
SDL12_COMPAT_SITE = https://github.com/libsdl-org/sdl12-compat.git
SDL12_COMPAT_SITE_METHOD = git
SDL12_COMPAT_DEPENDENCIES = sdl2
SDL12_COMPAT_LICENSE = custom
SDL12_COMPAT_LICENSE_FILES = LICENSE.txt
SDL12_COMPAT_INSTALL_STAGING = YES

SDL12_COMPAT_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
SDL12_COMPAT_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
SDL12_COMPAT_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_SO)"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_SO)"
SDL12_COMPAT_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_SO)"
SDL12_COMPAT_CONF_OPTS += -DSDL12TESTS=OFF

define SDL12_COMPAT_INSTALL_TARGET_CMDS
	cp --no-dereference --preserve=links -u $(@D)/libSDL* $(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))
