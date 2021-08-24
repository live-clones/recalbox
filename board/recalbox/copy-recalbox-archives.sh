#!/bin/bash -e

# PWD = buildroot dir
# BASE_DIR = output/ dir
# BUILD_DIR = output/build
# HOST_DIR = output/host
# BINARIES_DIR = output/images
# TARGET_DIR = output/target

# XU4 SD/EMMC CARD
#
#       1      31      63          1503     2015    2047
# +-----+-------+-------+-----------+--------+-------+--------+----------+--------------+
# | MBR |  bl1  |  bl2  |   uboot   |  tzsw  | erase |  BOOT  |  ROOTFS  |     FREE     |
# +-----+-------+-------+-----------+--------+-------+--------+----------+--------------+
#      512     15K     31K         751K    1007K   1023K     64M        1.2G
#
# https://wiki.odroid.com/odroid-xu4/software/partition_table
# https://github.com/hardkernel/u-boot/blob/odroidxu4-v2017.05/sd_fuse/sd_fusing.sh

xu4_fusing() {
    BINARIES_DIR=$1
    RECALBOXIMG=$2

    # fusing
    signed_bl1_position=1
    bl2_position=31
    uboot_position=63
    tzsw_position=1503
    env_position=2015

    echo "BL1 fusing"
    dd if="${BINARIES_DIR}/bl1.bin.hardkernel"  of="${RECALBOXIMG}" seek=$signed_bl1_position conv=notrunc || return 1

    echo "BL2 fusing"
    dd if="${BINARIES_DIR}/bl2.bin.hardkernel.720k_uboot"    of="${RECALBOXIMG}" seek=$bl2_position        conv=notrunc || return 1

    echo "u-boot fusing"
    dd if="${BINARIES_DIR}/u-boot.bin.hardkernel" of="${RECALBOXIMG}" seek=$uboot_position      conv=notrunc || return 1

    echo "TrustZone S/W fusing"
    dd if="${BINARIES_DIR}/tzsw.bin.hardkernel"   of="${RECALBOXIMG}" seek=$tzsw_position       conv=notrunc || return 1

    echo "u-boot env erase"
    dd if=/dev/zero of="${RECALBOXIMG}" seek=$env_position count=32 bs=512 conv=notrunc || return 1
}

# GO2 SD CARD
#
# 0s    1           16384  24576    32768     262144
# +-----+------------+-------+---------+--------+----------+-----------------+
# | MBR | idbloadder | uboot |  trust  |  BOOT  |  ROOTFS  |     OVERLAY     |
# +-----+------------+-------+---------+--------+----------+-----------------+
# 0b 512    ~4M         4M       4M       112M
#
# https://wiki.odroid.com/odroid-xu4/software/partition_table
# https://github.com/hardkernel/u-boot/blob/odroidxu4-v2017.05/sd_fuse/sd_fusing.sh
#sudo dd if=$IDBLOADER of=$1 conv=fsync bs=512 seek=64
#sudo dd if=$UBOOT of=$1 conv=fsync bs=512 seek=16384
#sudo dd if=$TRUST of=$1 conv=fsync bs=512 seek=24576 
odroidgo2_fusing() {
    BINARIES_DIR=$1
    RECALBOXIMG=$2

    # fusing
    idbloader_position=64
    uboot_position=16384
    trust_position=24576

    echo "IDBLOADER fusing"
    dd if="${BINARIES_DIR}/idbloader.img" of="${RECALBOXIMG}" seek=$idbloader_position conv=notrunc bs=512 || return 1

    echo "uboot fusing"
    dd if="${BINARIES_DIR}/uboot.img"     of="${RECALBOXIMG}" seek=$uboot_position conv=notrunc bs=512 || return 1

    echo "Trust fusing"
    dd if="${BINARIES_DIR}/trust.img"     of="${RECALBOXIMG}" seek=$trust_position conv=notrunc bs=512 || return 1
}
# $1 boot directory
generate_boot_file_list() {
  pushd "$1" >/dev/null
  find . -type f -printf '%P\n'
  popd >/dev/null
}


RECALBOX_BINARIES_DIR="${BINARIES_DIR}/recalbox"
RECALBOX_TARGET_DIR="${TARGET_DIR}/recalbox"

if [ -d "${RECALBOX_BINARIES_DIR}" ]; then
    rm -rf "${RECALBOX_BINARIES_DIR}"
fi

mkdir -p "${RECALBOX_BINARIES_DIR}"

# XU4, RPI0, RPI1, RPI2 or RPI3
RECALBOX_TARGET=$(grep -E "^BR2_PACKAGE_RECALBOX_TARGET_[A-Z_0-9]*=y$" "${BR2_CONFIG}" | sed -e s+'^BR2_PACKAGE_RECALBOX_TARGET_\([A-Z_0-9]*\)=y$'+'\1'+)

RECALBOX_TARGET_LOWER=$(echo ${RECALBOX_TARGET} | tr '[:upper:]' '[:lower:]')

RECALBOX_IMG="${RECALBOX_BINARIES_DIR}/recalbox-${RECALBOX_TARGET_LOWER}.img"

echo -e "\n----- Generating images/recalbox files -----\n"

case "${RECALBOX_TARGET}" in
    RPI0|RPI1|RPI2|RPI3|RPI4)
	# /boot
	echo "generating boot"
	cp -f "${BINARIES_DIR}/"*.dtb "${BINARIES_DIR}/rpi-firmware"
	cp -r "${BINARIES_DIR}/overlays" "${BINARIES_DIR}/rpi-firmware/" || exit 1
	rm -rf "${BINARIES_DIR}/rpi-firmware/boot" || exit 1
	mkdir -p "${BINARIES_DIR}/rpi-firmware/boot" || exit 1
	cp "${BINARIES_DIR}/zImage" "${BINARIES_DIR}/rpi-firmware/boot/linux" || exit 1
	cp "${BINARIES_DIR}/initrd.gz" "${BINARIES_DIR}/rpi-firmware/boot" || exit 1
	cp "${BINARIES_DIR}/rootfs.squashfs" "${BINARIES_DIR}/rpi-firmware/boot/recalbox" || exit 1
  [[ -f ${BINARIES_DIR}/pre-upgrade.sh ]] && \
    cp "${BINARIES_DIR}/pre-upgrade.sh" "${BINARIES_DIR}/rpi-firmware/pre-upgrade.sh"

	generate_boot_file_list "${BINARIES_DIR}/rpi-firmware/" | \
		grep -v -E '^(boot.lst|recalbox-user-config.txt|recalbox-boot.conf)$' >"${BINARIES_DIR}/rpi-firmware/boot.lst"

	# recalbox.tar.xz (formerly boot.tar.xz)
	tar -C "${BINARIES_DIR}/rpi-firmware" -cJf "${RECALBOX_BINARIES_DIR}/recalbox-${RECALBOX_TARGET_LOWER}.tar.xz" . ||
		{ echo "ERROR : unable to create recalbox.tar.xz" && exit 1 ; }

	#recalbox.img
	GENIMAGE_TMP="${BUILD_DIR}/genimage.tmp"
	rm -rf "${GENIMAGE_TMP}" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/rpi/genimage.cfg" "${BINARIES_DIR}/genimage.cfg.tmp" || exit 1
	FILES=$(find "${BINARIES_DIR}/rpi-firmware" -type f | sed -e s+"^${BINARIES_DIR}/rpi-firmware/\(.*\)$"+"file \1 \{ image = 'rpi-firmware/\1' }"+ | tr '\n' '@')
	cat "${BINARIES_DIR}/genimage.cfg.tmp" | sed -e s+'@files'+"${FILES}"+ | tr '@' '\n' > "${BINARIES_DIR}/genimage.cfg" || exit 1
	rm -f "${BINARIES_DIR}/genimage.cfg.tmp" || exit 1
	echo "generating image"
	genimage --rootpath="${TARGET_DIR}" --inputpath="${BINARIES_DIR}" --outputpath="${RECALBOX_BINARIES_DIR}" --config="${BINARIES_DIR}/genimage.cfg" --tmppath="${GENIMAGE_TMP}" || exit 1
	rm -f "${RECALBOX_BINARIES_DIR}/boot.vfat" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.tar" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.squashfs" || exit 1
	sync || exit 1
	;;

	ODROIDXU4)
	rm -rf "${BINARIES_DIR}/odroidxu4-firmware/boot" || exit 1
	mkdir -p "${BINARIES_DIR}/odroidxu4-firmware/boot" || exit 1

	# /boot
	echo "generating boot"
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidxu4/boot.ini" "${BINARIES_DIR}/odroidxu4-firmware/boot.ini" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidxu4/config.ini" "${BINARIES_DIR}/odroidxu4-firmware/config.ini" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidxu4/boot.ini" "${BINARIES_DIR}/odroidxu4-firmware/boot.ini.sample" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidxu4/config.ini" "${BINARIES_DIR}/odroidxu4-firmware/config.ini.sample" || exit 1
	mkdir -p "${BINARIES_DIR}/odroidxu4-firmware/overlays"
	for overlay in ads7846 hktft32 hktft35 hktft-cs-ogst \
	             i2c0 i2c1 spi0 sx865x-i2c1 uart0; do
		cp "${BINARIES_DIR}/${overlay}.dtb" "${BINARIES_DIR}/odroidxu4-firmware/overlays/${overlay}.dtbo" || exit 1
	done
	cp "${BINARIES_DIR}/exynos5422-odroidxu4.dtb" "${BINARIES_DIR}/odroidxu4-firmware/boot" || exit 1
	cp "${BINARIES_DIR}/uInitrd" "${BINARIES_DIR}/odroidxu4-firmware/boot/" || exit 1
	cp "${BINARIES_DIR}/zImage" "${BINARIES_DIR}/odroidxu4-firmware/boot/linux" || exit 1
	cp "${BINARIES_DIR}/rootfs.squashfs" "${BINARIES_DIR}/odroidxu4-firmware/boot/recalbox" || exit 1
  [[ -f ${BINARIES_DIR}/pre-upgrade.sh ]] && \
    cp "${BINARIES_DIR}/pre-upgrade.sh" "${BINARIES_DIR}/odroidxu4-firmware/pre-upgrade.sh"

	generate_boot_file_list "${BINARIES_DIR}/odroidxu4-firmware/" | \
		grep -v -E '^(boot.lst|boot.ini|config.ini|recalbox-boot.conf)$' >"${BINARIES_DIR}/boot.lst"

	# boot.tar.xz
	tar -C "${BINARIES_DIR}/odroidxu4-firmware" -cJf "${RECALBOX_BINARIES_DIR}/boot.tar.xz" . ||
		{ echo "ERROR : unable to create boot.tar.xz" && exit 1 ; }

	# recalbox.img
	GENIMAGE_TMP="${BUILD_DIR}/genimage.tmp"
	rm -rf "${GENIMAGE_TMP}" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidxu4/genimage.cfg" "${BINARIES_DIR}" || exit 1
	echo "generating image"
	genimage --rootpath="${TARGET_DIR}" --inputpath="${BINARIES_DIR}" --outputpath="${RECALBOX_BINARIES_DIR}" --config="${BINARIES_DIR}/genimage.cfg" --tmppath="${GENIMAGE_TMP}" || exit 1
	rm -f "${RECALBOX_BINARIES_DIR}/boot.vfat" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.tar" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.squashfs" || exit 1
	xu4_fusing "${BINARIES_DIR}" "${RECALBOX_BINARIES_DIR}/recalbox.img" || exit 1
	sync || exit 1
	;;

	ODROIDGO2)
	rm -rf "${BINARIES_DIR}/odroidgo2-firmware/boot" || exit 1
	mkdir -p "${BINARIES_DIR}/odroidgo2-firmware/boot" || exit 1

	# /boot
	echo "generating boot"
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidgo2/boot.ini" "${BINARIES_DIR}/odroidgo2-firmware/boot.ini" || exit 1
	cp "${BINARIES_DIR}/rk3326-odroidgo2-linux-v11.dtb" "${BINARIES_DIR}/odroidgo2-firmware" || exit 1
	cp "${BINARIES_DIR}/rk3326-odroidgo2-linux.dtb" "${BINARIES_DIR}/odroidgo2-firmware" || exit 1
	cp "${BINARIES_DIR}/rk3326-odroidgo3-linux.dtb" "${BINARIES_DIR}/odroidgo2-firmware" || exit 1
	cp "${BINARIES_DIR}/uInitrd" "${BINARIES_DIR}/odroidgo2-firmware/boot/" || exit 1
	cp "${BINARIES_DIR}/Image" "${BINARIES_DIR}/odroidgo2-firmware/boot/linux" || exit 1
	cp "${BINARIES_DIR}/rootfs.squashfs" "${BINARIES_DIR}/odroidgo2-firmware/boot/recalbox" || exit 1
  [[ -f ${BINARIES_DIR}/pre-upgrade.sh ]] && \
    cp "${BINARIES_DIR}/pre-upgrade.sh" "${BINARIES_DIR}/odroidgo2-firmware/pre-upgrade.sh"

	generate_boot_file_list "${BINARIES_DIR}/odroidgo2-firmware/" | \
		grep -v -E '^(boot.lst|recalbox-boot.conf)$' >"${BINARIES_DIR}/boot.lst"

	# boot.tar.xz
	tar -C "${BINARIES_DIR}/odroidgo2-firmware" -cJf "${RECALBOX_BINARIES_DIR}/boot.tar.xz" . ||
		{ echo "ERROR : unable to create boot.tar.xz" && exit 1 ; }

	# recalbox.img
	GENIMAGE_TMP="${BUILD_DIR}/genimage.tmp"
	rm -rf "${GENIMAGE_TMP}" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/odroidgo2/genimage.cfg" "${BINARIES_DIR}" || exit 1
	echo "generating image"
	genimage --rootpath="${TARGET_DIR}" --inputpath="${BINARIES_DIR}" --outputpath="${RECALBOX_BINARIES_DIR}" --config="${BINARIES_DIR}/genimage.cfg" --tmppath="${GENIMAGE_TMP}" || exit 1
	rm -f "${RECALBOX_BINARIES_DIR}/boot.vfat" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.tar" || exit 1
	rm -f "${BINARIES_DIR}/rootfs.squashfs" || exit 1
	odroidgo2_fusing "${BINARIES_DIR}" "${RECALBOX_BINARIES_DIR}/recalbox.img" || exit 1
	sync || exit 1
	;;

	X86|X86_64)
	# /boot
	rm -rf ${BINARIES_DIR}/pc-boot/boot || exit 1
	mkdir -p ${BINARIES_DIR}/pc-boot/boot/grub || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/grub2/grub.cfg" ${BINARIES_DIR}/pc-boot/boot/grub/grub.cfg || exit 1
	cp "${BINARIES_DIR}/bzImage" "${BINARIES_DIR}/pc-boot/boot/linux" || exit 1
	cp "${BINARIES_DIR}/initrd.gz" "${BINARIES_DIR}/pc-boot/boot" || exit 1
	cp "${BINARIES_DIR}/rootfs.squashfs" "${BINARIES_DIR}/pc-boot/boot/recalbox" || exit 1
  [[ -f ${BINARIES_DIR}/pre-upgrade.sh ]] && \
    cp "${BINARIES_DIR}/pre-upgrade.sh" "${BINARIES_DIR}/pc-boot/pre-upgrade.sh"

	# get UEFI files
	if [[ ${RECALBOX_TARGET} == "X86_64" ]] ; then
		mkdir -p "${BINARIES_DIR}/pc-boot/EFI/BOOT" || exit 1
		cp "${BINARIES_DIR}/bootia32.efi" "${BINARIES_DIR}/pc-boot/EFI/BOOT" || exit 1
		cp "${BINARIES_DIR}/bootx64.efi" "${BINARIES_DIR}/pc-boot/EFI/BOOT" || exit 1
		cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/grub2/grub.cfg" "${BINARIES_DIR}/pc-boot/EFI/BOOT" || exit 1
		genimg=genimage-x86-64.cfg
	else
		genimg=genimage-x86.cfg
	fi

	generate_boot_file_list "${BINARIES_DIR}/pc-boot/" | \
		grep -v -E '^(boot.lst|recalbox-boot.conf)$' >"${BINARIES_DIR}/pc-boot/boot.lst"

	# boot.tar.xz
	tar -C "${BINARIES_DIR}/pc-boot" -cJf "${RECALBOX_BINARIES_DIR}/boot.tar.xz" . ||
		{ echo "ERROR : unable to create boot.tar.xz" && exit 1 ; }

	# recalbox.img
	cp "${HOST_DIR}/usr/lib/grub/i386-pc/boot.img" "${BINARIES_DIR}/" || exit 1
	GENIMAGE_TMP="${BUILD_DIR}/genimage.tmp"
	rm -rf "${GENIMAGE_TMP}" || exit 1
	cp "${BR2_EXTERNAL_RECALBOX_PATH}/board/recalbox/grub2/${genimg}" "${BINARIES_DIR}/genimage.cfg" || exit 1
	echo "generating image"
	genimage --rootpath="${TARGET_DIR}" --inputpath="${BINARIES_DIR}" --outputpath="${RECALBOX_BINARIES_DIR}" --config="${BINARIES_DIR}/genimage.cfg" --tmppath="${GENIMAGE_TMP}" || exit 1

        rm -f "${RECALBOX_BINARIES_DIR}/boot.vfat" || exit 1
        rm -f "${BINARIES_DIR}/rootfs.tar" || exit 1
        rm -f "${BINARIES_DIR}/rootfs.squashfs" || exit 1
	sync || exit 1
	;;

    *)
        echo "Outch. Unknown target ${RECALBOX_TARGET} (see copy-recalbox-archives.sh)" >&2
        bash
        exit 1
esac

# Compress the generated .img
if mv -f ${RECALBOX_BINARIES_DIR}/recalbox.img ${RECALBOX_IMG} ; then
    echo "Compressing ${RECALBOX_IMG} ... "
    xz -9 -e --threads=0 "${RECALBOX_IMG}"
else
    echo "Couldn't move recalbox.img or compress it"
    exit 1
fi

# Computing hash sums to make have an update that can be dropped on a running Recalbox
echo "Computing sha1 sums ..."
(cd "${RECALBOX_BINARIES_DIR}" && for file in * ; do sha1sum "${file}" > "${file}.sha1"; done)
