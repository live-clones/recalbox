################################################################################
#
# libpcap
#
################################################################################

LIBPCAP_OVERRIDEN_VERSION = 1.9.0
LIBPCAP_OVERRIDEN_SITE = http://www.tcpdump.org/release
LIBPCAP_OVERRIDEN_SOURCE = libpcap-$(LIBPCAP_OVERRIDEN_VERSION).tar.gz
LIBPCAP_OVERRIDEN_LICENSE = BSD-3-Clause
LIBPCAP_OVERRIDEN_LICENSE_FILES = LICENSE
LIBPCAP_OVERRIDEN_INSTALL_STAGING = YES
LIBPCAP_OVERRIDEN_DEPENDENCIES = zlib host-flex host-bison

LIBPCAP_OVERRIDEN_CONF_ENV = \
	ac_cv_header_linux_wireless_h=yes \
	CFLAGS="$(LIBPCAP_CFLAGS)"
LIBPCAP_OVERRIDEN_CFLAGS = $(TARGET_CFLAGS)
LIBPCAP_OVERRIDEN_CONF_OPTS = --disable-yydebug --with-pcap=linux --without-dag
LIBPCAP_OVERRIDEN_CONFIG_SCRIPTS = pcap-config

# Omit -rpath from pcap-config output
define LIBPCAP_OVERRIDEN_CONFIG_REMOVE_RPATH
	$(SED) 's/^V_RPATH_OPT=.*/V_RPATH_OPT=""/g' $(@D)/pcap-config
endef
LIBPCAP_OVERRIDEN_POST_BUILD_HOOKS = LIBPCAP_OVERRIDEN_CONFIG_REMOVE_RPATH

# On purpose, not compatible with bluez5
ifeq ($(BR2_PACKAGE_BLUEZ_UTILS),y)
LIBPCAP_OVERRIDEN_DEPENDENCIES += bluez_utils
else
LIBPCAP_OVERRIDEN_CONF_OPTS += --disable-bluetooth
endif

ifeq ($(BR2_PACKAGE_DBUS),y)
LIBPCAP_OVERRIDEN_CONF_OPTS += --enable-dbus
LIBPCAP_OVERRIDEN_DEPENDENCIES += dbus
else
LIBPCAP_OVERRIDEN_CONF_OPTS += --disable-dbus
endif

ifeq ($(BR2_PACKAGE_LIBUSB),y)
LIBPCAP_OVERRIDEN_CONF_OPTS += --enable-canusb
LIBPCAP_OVERRIDEN_DEPENDENCIES += libusb
else
LIBPCAP_OVERRIDEN_CONF_OPTS += --disable-canusb
endif

ifeq ($(BR2_PACKAGE_LIBNL),y)
LIBPCAP_OVERRIDEN_DEPENDENCIES += libnl
LIBPCAP_OVERRIDEN_CFLAGS += "-I$(STAGING_DIR)/usr/include/libnl3"
LIBPCAP_OVERRIDEN_CONF_OPTS += --with-libnl=$(STAGING_DIR)/usr
else
LIBPCAP_OVERRIDEN_CONF_OPTS += --without-libnl
endif

# microblaze/sparc/sparc64 need -fPIC instead of -fpic
ifeq ($(BR2_microblaze)$(BR2_sparc)$(BR2_sparc64),y)
LIBPCAP_OVERRIDEN_CFLAGS += -fPIC
endif

$(eval $(autotools-package))
