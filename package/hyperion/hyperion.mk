################################################################################
#
# Hyperion
#
################################################################################

HYPERION_SITE = git://github.com/hyperion-project/hyperion.git
HYPERION_VERSION = 205f2fb30e23bdce83f7271c25ffd4b79aedc43e
HYPERION_LICENSE = MIT
HYPERION_SITE_METHOD = git
HYPERION_GIT_SUBMODULES = YES

HYPERION_CONF_OPTS += -DBCM_INCLUDE_DIR="$(STAGING_DIR)/usr/" \
	-DBCM_LIBRARIES="$(STAGING_DIR)/usr/lib/" \
	-DDispmanx_LIBRARIES="bcm_host" \
	-DCMAKE_BUILD_TYPE=Release \
	-DPLATFORM="rpi" \
	-Wno-dev \
	-DUSE_SYSTEM_PROTO_LIBS=ON \
	-DPROTOBUF_PROTOC_EXECUTABLE="$(HOST_DIR)/bin/protoc" \
	-DENABLE_OPENCV=OFF \
	-DENABLE_QT5=ON \
	--build "$(@D)/output/" "$(@D)/"

HYPERION_DEPENDENCIES += libusb qt5base host-libusb rpi-firmware host-protobuf host-cmake

# rpi0, 1, 2 and 3
ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
HYPERION_DEPENDENCIES += rpi-userland
endif

define HYPERION_PROTOBUF_COMPILE
	mkdir -p "$(@D)/host-compile/"
	cd "$(@D)/host-compile/" && $(HOST_MAKE_ENV) $(HOST_CONFIGURE_OPTS) $(HOST_DIR)/usr/bin/cmake "$(@D)/" \
		-DENABLE_DISPMANX=OFF -DENABLE_OPENCV=OFF -DENABLE_QT5=ON --build "$(@D)/host-compile/" "$(@D)/"
endef
define HYPERION_RPI_FIXUP
	sed -i 's/hyperion//' $(@D)/libsrc/effectengine/CMakeLists.txt
	sed -i 's/target_link_libraries(hyperiond dispmanx-grabber)/target_link_libraries(hyperiond dispmanx-grabber bcm_host)/' $(@D)/src/hyperiond/CMakeLists.txt
endef
HYPERION_PRE_CONFIGURE_HOOKS += HYPERION_PROTOBUF_COMPILE
HYPERION_PRE_CONFIGURE_HOOKS += HYPERION_RPI_FIXUP

define HYPERION_INSTALL_LIBS
	$(INSTALL) -D -m 0755 $(@D)/lib/*so $(TARGET_DIR)/usr/lib
endef
HYPERION_POST_INSTALL_TARGET_HOOKS += HYPERION_INSTALL_LIBS

$(eval $(cmake-package))
