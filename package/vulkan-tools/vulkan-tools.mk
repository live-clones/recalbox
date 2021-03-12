################################################################################
#
# vulkan-tols
#
################################################################################

VULKAN_TOOLS_VERSION = v1.2.171
VULKAN_TOOLS_SITE = $(call github,KhronosGroup,Vulkan-Tools,$(VULKAN_TOOLS_VERSION))
VULKAN_TOOLS_LICENSE = Apache-2.0
VULKAN_TOOLS_LICENSE_FILES = LICENSE.txt
VULKAN_TOOLS_INSTALL_STAGING = YES
VULKAN_TOOLS_SUBDIR = vulkaninfo

VULKAN_TOOLS_CONF_OPTS = -DBUILD_WSI_XCB_SUPPORT=OFF \
												 -DBUILD_WSI_XLIB_SUPPORT=OFF \
												 -DBUILD_WSI_WAYLAND_SUPPORT=OFF

$(eval $(cmake-package))
