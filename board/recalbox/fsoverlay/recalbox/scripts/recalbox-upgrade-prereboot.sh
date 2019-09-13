# Since S11upgrade cannot update itself during the update process
# it calls this script once the update is done and before rebooting
# giving a chance to apply some required changes

source /recalbox/scripts/recalbox-includes.sh

# If we don't restore before reboot, default values will be saved at reboot, losing custom ones
recallog "Restoring sound config before reboot"
alsactl -f /recalbox/share/system/configs/asound.state restore 2>&1 | recallog

recallog "Upgrading RA Core names"
upgradeRetroarchCoreNames -f /recalbox/share/system/configs/retroarch.corenames 2>&1 | recallog

recallog "Clean up init scripts"
rm -f /etc/init.d/S02splash
rm -f /etc/init.d/S03populate
rm -f /etc/init.d/S04populate
rm -f /etc/init.d/S05acpid

recallog "Reset case"
sed -i 's/^case=.*$/d' /boot/recalbox-boot.conf
