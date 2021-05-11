################################################################################
#
# recalbox-x86_64_efi derived from grub2
#
################################################################################

RECALBOX_X86_64_EFI_VERSION = 2.04
RECALBOX_X86_64_EFI_SITE = http://ftp.gnu.org/gnu/grub
RECALBOX_X86_64_EFI_SOURCE = grub-$(RECALBOX_X86_64_EFI_VERSION).tar.xz
RECALBOX_X86_64_EFI_LICENSE = GPL-3.0+
RECALBOX_X86_64_EFI_LICENSE_FILES = COPYING
RECALBOX_X86_64_EFI_DEPENDENCIES = host-bison host-flex host-grub2
HOST_RECALBOX_X86_64_EFI_DEPENDENCIES = host-bison host-flex
RECALBOX_X86_64_EFI_INSTALL_IMAGES = YES

# 0001-build-Fix-GRUB-i386-pc-build-with-Ubuntu-gcc.patch
define RECALBOX_X86_64_EFI_AVOID_AUTORECONF
	$(Q)touch $(@D)/Makefile.util.am
	$(Q)touch $(@D)/Makefile.in
endef
RECALBOX_X86_64_EFI_POST_PATCH_HOOKS += RECALBOX_X86_64_EFI_AVOID_AUTORECONF
HOST_RECALBOX_X86_64_EFI_POST_PATCH_HOOKS += RECALBOX_X86_64_EFI_AVOID_AUTORECONF

# 0002-yylex-Make-lexer-fatal-errors-actually-be-fatal.patch
RECALBOX_X86_64_EFI_IGNORE_CVES += CVE-2020-10713
# 0005-calloc-Use-calloc-at-most-places.patch
RECALBOX_X86_64_EFI_IGNORE_CVES += CVE-2020-14308
# 0006-malloc-Use-overflow-checking-primitives-where-we-do-.patch
RECALBOX_X86_64_EFI_IGNORE_CVES += CVE-2020-14309 CVE-2020-14310 CVE-2020-14311
# 0019-script-Avoid-a-use-after-free-when-redefining-a-func.patch
RECALBOX_X86_64_EFI_IGNORE_CVES += CVE-2020-15706
# 0028-linux-Fix-integer-overflows-in-initrd-size-handling.patch
RECALBOX_X86_64_EFI_IGNORE_CVES += CVE-2020-15707

RECALBOX_X86_64_EFI_BUILTIN_MODULES = boot linux ext2 fat squash4 part_msdos part_gpt normal efi_gop

RECALBOX_X86_64_EFI_IMAGE = $(BINARIES_DIR)/bootx64.efi
RECALBOX_X86_64_EFI_PREFIX = /EFI/BOOT
RECALBOX_X86_64_EFI_TUPLE = x86_64-efi
RECALBOX_X86_64_EFI_TARGET = x86_64
RECALBOX_X86_64_EFI_PLATFORM = efi

# Grub2 is kind of special: it considers CC, LD and so on to be the
# tools to build the host programs and uses TARGET_CC, TARGET_CFLAGS,
# TARGET_CPPFLAGS, TARGET_LDFLAGS to build the bootloader itself.
#
# NOTE: TARGET_STRIP is overridden by !BR2_STRIP_strip, so always
# use the cross compile variant to ensure grub2 builds

HOST_RECALBOX_X86_64_EFI_CONF_ENV = \
	CPP="$(HOSTCC) -E"

RECALBOX_X86_64_EFI_CONF_ENV = \
	CPP="$(TARGET_CC) -E" \
	TARGET_CC="$(TARGET_CC)" \
	CFLAGS=" $(TARGET_CFLAGS) -O2" \
	CPPFLAGS="$(TARGET_CPPFLAGS) -O2 -fno-stack-protector" \
	LDFLAGS="$(TARGET_LDFLAGS) -O2" \
	TARGET_NM="$(TARGET_NM)" \
	TARGET_OBJCOPY="$(TARGET_OBJCOPY)" \
	TARGET_STRIP="$(TARGET_CROSS)strip"

RECALBOX_X86_64_EFI_CONF_OPTS = \
	--target=$(RECALBOX_X86_64_EFI_TARGET) \
	--with-platform=$(RECALBOX_X86_64_EFI_PLATFORM) \
	--prefix=/ \
	--exec-prefix=/ \
	--disable-grub-mkfont \
	--enable-efiemu=no \
	ac_cv_lib_lzma_lzma_code=no \
	--enable-device-mapper=no \
	--enable-libzfs=no \
	--disable-werror

HOST_RECALBOX_X86_64_EFI_CONF_OPTS = \
	--disable-grub-mkfont \
	--enable-efiemu=no \
	ac_cv_lib_lzma_lzma_code=no \
	--enable-device-mapper=no \
	--enable-libzfs=no \
	--disable-werror

define RECALBOX_X86_64_EFI_INSTALL_IMAGES_CMDS
	mkdir -p $(dir $(RECALBOX_X86_64_EFI_IMAGE))
	$(HOST_DIR)/usr/bin/grub-mkimage \
		-d $(@D)/grub-core/ \
		-O $(RECALBOX_X86_64_EFI_TUPLE) \
		-o $(RECALBOX_X86_64_EFI_IMAGE) \
		-p "$(RECALBOX_X86_64_EFI_PREFIX)" \
		$(RECALBOX_X86_64_EFI_BUILTIN_MODULES)
endef

$(eval $(autotools-package))
$(eval $(host-autotools-package))
