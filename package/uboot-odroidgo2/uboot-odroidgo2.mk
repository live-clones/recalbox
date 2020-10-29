################################################################################
#
# uboot-xu4
#
################################################################################

UBOOT_ODROIDGO2_VERSION = odroidgoA-v2017.09
UBOOT_ODROIDGO2_SITE = $(call github,hardkernel,u-boot,$(UBOOT_ODROIDGO2_VERSION))
UBOOT_ODROIDGO2_INSTALL_TARGET = NO
UBOOT_ODROIDGO2_INSTALL_IMAGES = YES

define UBOOT_ODROIDGO2_BUILD_CMDS
	cd $(@D) && \
	TOOLCHAIN_ARM64=`dirname $(TARGET_CC)` \
	ADDR2LINE_ARM64=aarch64-buildroot-linux-gnu-addr2line \
	OBJ_ARM64=aarch64-buildroot-linux-gnu-objdump \
	GCC_ARM64=aarch64-buildroot-linux-gnu- \
	./make.sh odroidgo2
endef

define UBOOT_ODROIDGO2_INSTALL_IMAGES_CMDS
	$(INSTALL) -D $(@D)/sd_fuse/uboot.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/sd_fuse/idbloader.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/sd_fuse/trust.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/sd_fuse/rk3326_header_miniloader_spiboot.img $(BINARIES_DIR)/
endef
$(eval $(generic-package))
