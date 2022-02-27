#!/bin/bash

global_interface=/var/run/wpa_supplicant_global
config_file=/recalbox/share/system/recalbox.conf
config_file_boot=/boot/recalbox-backup.conf
wpa_file=/overlay/.configs/wpa_supplicant.conf
INIT_SCRIPT=$(basename "$0")
system_setting="recalbox_settings"

# if /recalbox/share is not yet mounted
if ! [ -f "$config_file" ]; then
  # use the boot version of the file
  config_file="$config_file_boot"
fi

mask2cidr() {
    nbits=0
    IFS=.
    for dec in $1 ; do
        case $dec in
            255) let nbits+=8;;
            254) let nbits+=7;;
            252) let nbits+=6;;
            248) let nbits+=5;;
            240) let nbits+=4;;
            224) let nbits+=3;;
            192) let nbits+=2;;
            128) let nbits+=1;;
            0);;
            *) echo "Error: $dec is not recognised"; exit 1
        esac
    done
    echo "$nbits"
}

wpa_supplicant_command() {
  wpa_cli -g "${global_interface}" "$@"
}

# Function to create wifi profiles based on user settings
rb_wifi_configure() {
  [ "$1" = "1" ] && X="" || X="$1"
  local interface="$2"
  settings_ssid=$("$system_setting" -command load -key "wifi${X}.ssid" -source "$config_file")
  settings_key=$("$system_setting" -command load -key "wifi${X}.key" -source "$config_file")
  settings_ip=$("$system_setting" -command load -key "wifi${X}.ip" -source "$config_file")
  settings_gateway=$("$system_setting" -command load -key "wifi${X}.gateway" -source "$config_file")
  settings_netmask=$("$system_setting" -command load -key "wifi${X}.netmask" -source "$config_file")
  settings_nameservers=$("$system_setting" -command load -key "wifi${X}.nameservers" -source "$config_file")

  # setup wpa_supplicant network
  if [[ "$settings_ssid" != "" ]] ;then

    recallog -s "${INIT_SCRIPT}" -t "WIFI" "Configuring wifi for SSID: $settings_ssid"
    network=$(wpa_cli -i "$interface" add_network)
    wpa_cli -i "$interface" set_network "$network" ssid "\"$settings_ssid\"" || exit 1
    if [ -n "$settings_key" ]; then
        # Connect to protected wifi
        wpa_cli -i "$interface" set_network "$network" psk "\"$settings_key\"" || exit 1
        wpa_cli -i "$interface" set_network "$network" key_mgmt WPA-PSK WPA-EAP WPA-PSK-SHA256 NONE SAE || exit 1
        wpa_cli -i "$interface" set_network "$network" sae_password "\"$settings_key\"" || exit 1
        wpa_cli -i "$interface" set_network "$network" ieee80211w 1 || exit 1
    else
        # Connect to open ssid
        wpa_cli -i "$interface" set_network "$network" key_mgmt NONE || exit 1
    fi
    wpa_cli -i "$interface" set_network "$network" scan_ssid 1 || exit 1
    wpa_cli -i "$interface" enable_network "$network" || exit 1

    # static ip configuration in dhcpcd.conf
    sed -i "/\b\($interface\|static\)\b/d" /etc/dhcpcd.conf
    if [[ "$settings_ip" != "" ]] && \
      [[ "$settings_gateway" != "" ]] && \
      [[ "$settings_netmask" != "" ]] && \
      [[ "$settings_nameservers" != "" ]]; then
      recallog -s "${INIT_SCRIPT}" -t "WIFI" "static ip configuration"
      settings_netmask=$(mask2cidr "$settings_netmask")
      recallog -s "${INIT_SCRIPT}" -t "WIFI" "static ip_address=$settings_ip/$settings_netmask"
      recallog -s "${INIT_SCRIPT}" -t "WIFI" "static routers=$settings_gateway"
      recallog -s "${INIT_SCRIPT}" -t "WIFI" "static domain_name_servers=$settings_nameservers"
      {
        echo "interface $interface"
        echo "static ip_address=$settings_ip/$settings_netmask"
        echo "static routers=$settings_gateway"
        echo "static domain_name_servers=$settings_nameservers"
      } >> /etc/dhcpcd.conf
    fi
  fi
}

# function to drop all of the wpa_suppliment networks
cleanup_interface() {
  # clear wpa_supplicant configuration and rebuild
  local interface="$1"
  # limit any infinite loop to 10 max
  for i in {1..10}; do

    # get the last network id
    netid=$(wpa_cli -i "$interface" list_networks | tail -n1)
    netid=$(echo "$netid" | tr " " "\n")

    # exit at the header record
    if [[ $netid == network* ]]; then
      break
    fi

    # remove network id
    wpa_cli -i "$interface" remove_network "$netid" || exit 1
  done
}

configure_interface() {
  local interface="$1"
  wpa_cli -i "$interface" set update_config 1

  settings_region=$("$system_setting" -command load -key "wifi.region" -source "$config_file")
  wpa_cli -i "$interface" set country "$settings_region"

  # iterate through all network ...
  for i in {1..3}; do
    rb_wifi_configure "$i" "$interface"
  done

  # write wpa_supplicant configuration
  wpa_cli -i "$interface" save_config
}

# start wpa_supplicant
enable_wifi() {
  local interface
  pgrep wpa_supplicant >/dev/null || wpa_supplicant -B -M -i wlan* -c "$wpa_file" -Dnl80211,wext -u -f /var/log/wpa_supplicant.log -g "${global_interface}"
  while ! wpa_cli -g "${global_interface}" interface; do
    sleep 0.2
  done
  # enable wifi for plugged interface
  wpa_cli -g /var/run/wpa_supplicant_global interface | sed '1,/^Available interface/d' | while read -r interface; do
    configure_interface "$interface"
  done
}

# stop wpa_supplicant
disable_wifi() {
  wpa_supplicant_command terminate
  local COUNT=50
  while pgrep wpa_supplicant >/dev/null; do
    sleep 0.2;
    COUNT=$((COUNT-1))
    if [ "$COUNT" -eq 0 ]; then
      pkill -9 wpa_supplicant
      break
    fi
  done
}
