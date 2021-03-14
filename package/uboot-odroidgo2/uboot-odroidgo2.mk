################################################################################
#
# uboot-xu4
#
################################################################################

UBOOT_ODROIDGO2_VERSION = e9d7bedabe68cec4d5924ee11de510399577ece2
UBOOT_ODROIDGO2_SITE = $(call github,hardkernel,u-boot,$(UBOOT_ODROIDGO2_VERSION))
UBOOT_ODROIDGO2_LICENSE = GPL-2.0+
UBOOT_ODROIDGO2_LICENSE_FILES = Licenses/gpl-2.0.txt

UBOOT_ODROIDGO2_INSTALL_TARGET = NO
UBOOT_ODROIDGO2_INSTALL_IMAGES = YES

define UBOOT_ODROIDGO2_BUILD_CMDS
	cd $(@D) && \
	TOOLCHAIN_ARM64=`dirname $(TARGET_CC)` \
	ADDR2LINE_ARM64=aarch64-buildroot-linux-gnu-addr2line \
	OBJ_ARM64=aarch64-buildroot-linux-gnu-objdump \
	GCC_ARM64=aarch64-buildroot-linux-gnu- \
	./make.sh odroidgoa
endef

define UBOOT_ODROIDGO2_INSTALL_IMAGES_CMDS
	$(INSTALL) -D $(@D)/sd_fuse/uboot.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/sd_fuse/idbloader.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/sd_fuse/trust.img $(BINARIES_DIR)/
endef
$(eval $(generic-package))
