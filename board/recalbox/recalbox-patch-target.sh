#!/bin/bash -e

# PWD = source dir
# BASE_DIR = build dir
# BUILD_DIR = base dir/build
# HOST_DIR = base dir/host
# BINARIES_DIR = images dir
# TARGET_DIR = target dir

RECALBOX_BINARIES_DIR="${BINARIES_DIR}/recalbox"
RECALBOX_TARGET_DIR="${TARGET_DIR}/recalbox"

sed -i "s|root:x:0:0:root:/root:/bin/sh|root:x:0:0:root:/recalbox/share/system:/bin/sh|g" "${TARGET_DIR}/etc/passwd" || exit 1
rm -rf "${TARGET_DIR}/etc/dropbear" || exit 1
ln -sf "/overlay/dropbear" "${TARGET_DIR}/etc/dropbear" || exit 1

rm -rf "${TARGET_DIR}/etc/timezone" || exit 1
ln -sf "/var/timezone" "${TARGET_DIR}/etc/timezone" || exit 1
rm -rf "${TARGET_DIR}/etc/localtime" || exit 1
ln -sf "/var/localtime" "${TARGET_DIR}/etc/localtime" || exit 1

mkdir -p ${TARGET_DIR}/etc/emulationstation || exit 1
ln -sf "/recalbox/share/cheats" "${TARGET_DIR}/recalbox/share_init/cheats/custom" || exit 1

# remove useless files
FILES_TO_REMOVE=("${TARGET_DIR}/lib/firmware/netronome")
for file in ${FILES_TO_REMOVE[*]}; do
  [ -e "$file" ] && rm -rf "$file"
done

# allow exfat share partition to mount. It needs a blank directory
rm -f "${TARGET_DIR}/recalbox/share/.keep" || exit 1

# we don't want the kodi startup script
rm -f "${TARGET_DIR}/etc/init.d/S50kodi" || exit 1

# we don't want default xorg files
rm -f "${TARGET_DIR}/etc/X11/xorg.conf" || exit 1

# we want an empty boot directory (grub installation copy some files in the target boot directory)
rm -rf "${TARGET_DIR}/boot/grub" || exit 1

# reorder the boot scripts for the network boot
if [ -e "${TARGET_DIR}/etc/init.d/S10udev" ]; then
    mv "${TARGET_DIR}/etc/init.d/S10udev" "${TARGET_DIR}/etc/init.d/S02udev" || exit 1 # move to make number spaces
fi
if [ -e "${TARGET_DIR}/etc/init.d/S30dbus" ]; then
    mv "${TARGET_DIR}/etc/init.d/S30dbus" "${TARGET_DIR}/etc/init.d/S06dbus" || exit 1 # move really before for network (connman prerequisite)
fi
if [ -e "${TARGET_DIR}/etc/init.d/S40network" ]; then
    mv "${TARGET_DIR}/etc/init.d/S40network" "${TARGET_DIR}/etc/init.d/S07network" || exit 1 # move to make ifaces up sooner, mainly mountable/unmountable before/after share
fi

# remove kodi default joystick configuration files
# while as a minimum, the file joystick.Sony.PLAYSTATION(R)3.Controller.xml makes references to PS4 controllers with axes which doesn't exist (making kodi crashing)
# i prefer to put it here than in packages/kodi while there are already a lot a lot of things
rm -rf "${TARGET_DIR}/usr/share/kodi/system/keymaps/joystick."*.xml || exit 1

# tmpfs or sysfs is mounted over theses directories
# clear these directories is required for the upgrade (otherwise, tar xf fails)
rm -rf "${TARGET_DIR}/"{var,run,sys,tmp} || exit 1
mkdir "${TARGET_DIR}/"{var,run,sys,tmp} || exit 1

# make /etc/shadow a file generated from /boot/recalbox-boot.conf for security
rm -f "${TARGET_DIR}/etc/shadow" || exit 1
ln -sf "/run/recalbox.shadow" "${TARGET_DIR}/etc/shadow" || exit 1

# Add the date while the version can be nightly or unstable
if [ ! -f "${TARGET_DIR}/recalbox/recalbox.version" ]; then
    echo "development" > "${TARGET_DIR}/recalbox/recalbox.version"
fi
RVERSION=$(cat "${TARGET_DIR}/recalbox/recalbox.version")

# bootsplash
TGVERSION="${RVERSION} $(date "+%Y/%m/%d %H:%M")"
convert "${TARGET_DIR}/recalbox/system/resources/splash/logo.png" -fill white -pointsize 30 -annotate +50+1020 "${TGVERSION}" "${TARGET_DIR}/recalbox/system/resources/splash/logo-version.png" || exit 1
convert "${TARGET_DIR}/recalbox/system/resources/splash/240p/logo.png" -fill white -pointsize 12 -annotate +8+170 "${TGVERSION}" "${TARGET_DIR}/recalbox/system/resources/splash/240p/logo-version.png" || exit 1
convert "${TARGET_DIR}/recalbox/system/resources/splash/logo.png" -fill white -pointsize 60 -gravity center -annotate +0+0 "Upgrading the system\nPlease wait..." "${TARGET_DIR}/recalbox/system/resources/splash/logo-upgrade.png" || exit 1

# Splash video subtitle
echo -e "1\n00:00:00,000 --> 00:00:03,000\n${TGVERSION}" > "${TARGET_DIR}/recalbox/system/resources/splash/recalboxintro.srt"
omx_fnt="/usr/share/fonts/dejavu/DejaVuSans-BoldOblique.ttf"
if [[ -f ${TARGET_DIR}$omx_fnt ]] ; then
    sed -i "s|omx_fnt=\"\"|omx_fnt=\"--font=$omx_fnt\"|g" "${TARGET_DIR}/etc/init.d/S06splash"
fi

# Add the initrd to pi only
#[ -e "${BINARIES_DIR}/rpi-firmware/config.txt" ] && ! grep -q "initrd initrd.gz" "${BINARIES_DIR}/rpi-firmware/config.txt" && echo "initrd initrd.gz" >> "${BINARIES_DIR}/rpi-firmware/config.txt"

# Changelog
cp "$BR2_EXTERNAL_RECALBOX_PATH/CHANGELOG.md" "${TARGET_DIR}/recalbox/recalbox.changelog"
cp "$BR2_EXTERNAL_RECALBOX_PATH/RELEASE-NOTES.md" "${TARGET_DIR}/recalbox/recalbox.releasenotes"

# Compress folders
# 
# BEWARE Developpers: Once the tar.xz is created, it is not updated.
# This does not matter when the build starts from scratch.
# However, while in development cycles, you may want to update one or more folders
# To do so, untar those files before running your next build.
for DIR in shaders
do
    FDIR="${TARGET_DIR}/recalbox/share_init/${DIR}"
    if test ! -e "${FDIR}.tar.gz"; then
        rm -f "${FDIR}.tar"
        cd "${TARGET_DIR}/recalbox/share_init" && \
            tar -cf "${DIR}.tar" "${DIR}" && \
            xz -f -9 -e --threads=0 "${DIR}.tar"
    fi
    rm -f "${FDIR}.tar"
    rm -rf "${FDIR}"
done

# Clear share directory, it must be empty or share cannot be mounted
if [ "$(find "${TARGET_DIR}/recalbox/share" | wc -l)" -gt 1 ]; then 
    echo "Files are present in ${TARGET_DIR}/recalbox/share, it must be empty or share cannot be mounted"
    exit 1
fi
