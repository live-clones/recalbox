################################################################################
#
# vulkan-loader
#
################################################################################

VULKAN_LOADER_VERSION = v1.2.171
VULKAN_LOADER_SITE = $(call github,KhronosGroup,Vulkan-Loader,$(VULKAN_LOADER_VERSION))
VULKAN_LOADER_LICENSE = Apache-2.0
VULKAN_LOADER_LICENSE_FILES = LICENSE.txt
VULKAN_LOADER_INSTALL_STAGING = YES

VULKAN_LOADER_CONF_OPTS = -DVulkanHeaders_INCLUDE_DIR=$(STAGING_DIR)/usr/include/ \
													-DVulkanRegistry_DIR=$(TARGET_DIR)/usr/share/vulkan/registry \
													-DBUILD_WSI_XCB_SUPPORT=OFF \
													-DBUILD_WSI_XLIB_SUPPORT=OFF \
													-DBUILD_WSI_WAYLAND_SUPPORT=OFF

$(eval $(cmake-package))
