#!/bin/sh


GTMP="/tmp"
DHOME="/recalbox/share/system"

# to be callable by any external tool
if test $# -eq 1
then
    REPORTNAME=$(basename "$1" | sed -e s+'^\([^.]*\)\..*$'+'\1'+)
    OUTPUTFILE=$1
else
    REPORTNAME="recalbox-support-"$(date +%Y%m%d%H%M%S)
    OUTPUTFILE="/recalbox/share/saves/${REPORTNAME}.tar.gz"
fi

TMPDIR="${GTMP}/${REPORTNAME}"

f_cp() {
    test -e "$1" && cp "$1" "$2"
}

d_cp() {
    test -d "$1" && cp -pr "$1" "$2"
}


if mkdir "${TMPDIR}" && mkdir "${TMPDIR}/"{system,joysticks,lirc,kodi}
then
    if ! cd "${TMPDIR}"
    then
	echo "Change directory failed" >&2
	exit 1
    fi
else
    echo "Reporting directory creation failed" >&2
    exit 1
fi

# SYSTEM
DSYSTEM="${TMPDIR}/system"
dmesg 	         	> "${DSYSTEM}/dmesg.txt"
lsmod 	         	> "${DSYSTEM}/lsmod.txt"
ps    	         	> "${DSYSTEM}/ps.txt"
df -h 	         	> "${DSYSTEM}/df.txt"
netstat -tuan    	> "${DSYSTEM}/netstat.txt"
lsusb -v         	> "${DSYSTEM}/lsusb.txt" 2>/dev/null
tvservice -m CEA 	> "${DSYSTEM}/tvservice-CEA.txt"
tvservice -m DMT 	> "${DSYSTEM}/tvservice-DMT.txt"
tvservice -s  		> "${DSYSTEM}/tvservice-status.txt"
ifconfig -a             > "${DSYSTEM}/ifconfig.txt"
lspci                   > "${DSYSTEM}/lspci.txt"
amixer                  > "${DSYSTEM}/amixer.txt"
aplay -l                > "${DSYSTEM}/aplay-l.txt"
DISPLAY=:0.0 glxinfo    > "${DSYSTEM}/glxinfo.txt"
DISPLAY=:0.0 xrandr     > "${DSYSTEM}/xrandr.txt"
DISPLAY=:0.0 xrandr -q  > "${DSYSTEM}/xrandr-q.txt"
connmanctl technologies > "${DSYSTEM}/connman-technologies.txt"
connmanctl services     > "${DSYSTEM}/connman-services.txt"
mount                   > "${DSYSTEM}/mount.txt"
blkid                   > "${DSYSTEM}/blkid.txt"
hciconfig               > "${DSYSTEM}/hciconfig.txt"
f_cp /recalbox/recalbox.version                               "${DSYSTEM}"
f_cp /recalbox/recalbox.arch                                  "${DSYSTEM}"
f_cp /boot/config.txt                                         "${DSYSTEM}"
f_cp /boot/recalbox-config.txt                                "${DSYSTEM}"
f_cp /recalbox/share/system/recalbox.conf                     "${DSYSTEM}"
d_cp /recalbox/share/system/logs                              "${DSYSTEM}"
f_cp /var/log/messages                                        "${DSYSTEM}"
f_cp /recalbox/share/system/.emulationstation/es_settings.cfg "${DSYSTEM}"
f_cp /recalbox/share/system/.emulationstation/es_log.txt      "${DSYSTEM}"
f_cp /recalbox/share/system/.emulationstation/es_input.cfg    "${DSYSTEM}"
f_cp /boot/recalbox-boot.conf                                 "${DSYSTEM}"
f_cp /var/log/Xorg.0.log                                      "${DSYSTEM}"
f_cp /recalbox/share_init/system/.emulationstation/es_systems.cfg "${DSYSTEM}/share_init_es_systems.cfg"
f_cp /recalbox/share/system/.emulationstation/es_systems.cfg "${DSYSTEM}/share_es_systems.cfg"
find /recalbox/share/roms/ -type f \( ! -iname "*.txt" ! -iname "*.xml" ! -iname "*.png" ! -iname "*.jpg" ! -iname "*.dat" \) ! -path "*/data/*" | wc -l > "${DSYSTEM}/approxnbroms.txt"

# Themes
ls -1 /recalbox/share/themes > "${DSYSTEM}/share_themes.txt"
ls -1 /recalbox/share/system/.emulationstation/themes > "${DSYSTEM}/share_es_themes.txt"
ls -1 /recalbox/share_init/system/.emulationstation/themes > "${DSYSTEM}/share_init_es_themes.txt"

# Recalbox intros
ls -l /recalbox/system/resources/splash/ | tail -n +2 | awk '{print $9, "-", $5}' > "${DSYSTEM}/recalbox_intros.txt"

# Update logs
d_cp /recalbox/share/system/upgrade                           "${DSYSTEM}"
find "${DSYSTEM}/upgrade" -type f ! -name "*upgrade*" | xargs rm

# Emulators configs
test -d "/recalbox/share/system/configs" && rsync -a "/recalbox/share/system/configs" "${TMPDIR}" --exclude "ppsspp/PSP/SAVEDATA" --exclude "ppsspp/PSP/PPSSPP_STATE" --exclude "retroarch/overlays"

# joysticks
DJOYS="${TMPDIR}/joysticks"
find /dev/input > "${DJOYS}/inputs.txt"
for J in /dev/input/event*
do
    N=$(basename ${J})
    evtest --info "${J}"          > "${DJOYS}/evtest.${N}.txt"
    udevadm info -q all -n "${J}" > "${DJOYS}/udevadm.${N}.txt"
done
sdl2-jstest -l > "${DJOYS}/sdl2-jstest.txt"


# lirc
DLIRC="${TMPDIR}/lirc"
f_cp "${DHOME}/.config/lirc/lircd.conf" "${DLIRC}"

# kodi
DKODI="${TMPDIR}/kodi"
f_cp "${DHOME}/.kodi/userdata/Lircmap.xml"          "${DKODI}"
f_cp "${DHOME}/.kodi/userdata/keymaps/recalbox.xml" "${DKODI}"
d_cp "${DHOME}/.kodi/userdata/remotes"              "${DKODI}"
d_cp "${DHOME}/.kodi/userdata/addon_data/peripheral.joystick/resources/buttonmaps/xml/linux"              "${DKODI}"
test -e "${DHOME}/.kodi/temp/kodi.log" && tail -4000 "${DHOME}/.kodi/temp/kodi.log" > "${DKODI}/kodi.log"

if ! (cd "${GTMP}" && tar cf -  "${REPORTNAME}" | gzip -c > "${OUTPUTFILE}")
then
    echo "Reporting zip creation failed" >&2
    exit 1
fi

rm -rf "${TMPDIR}"
echo "${OUTPUTFILE}"
exit 0
