################################################################################
#
# mali-t62x
#
################################################################################

MALI_T62X_VERSION = d4000def121b818ae0f583d8372d57643f723fdc
MALI_T62X_SITE = $(call github,LibreELEC,libmali,$(MALI_T62X_VERSION))
MALI_T62X_DEPENDENCIES = wayland
MALI_T62X_INSTALL_STAGING = YES
MALI_T62X_PROVIDES = libegl libgles
MALI_T62X_LICENSE = COPYRIGHT
MALI_T62X_LICENSE_FILES = END_USER_LICENCE_AGREEMENT.txt

MALI_T62X_CONF_OPTS = -DMALI_VARIANT=t620 -DMALI_ARCH=arm-linux-gnueabihf

$(eval $(cmake-package))
