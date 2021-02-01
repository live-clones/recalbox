################################################################################
#
# MEGATOOLS
#
################################################################################

MEGATOOLS_VERSION = 1.10.3
MEGATOOLS_SITE = https://megatools.megous.com/builds
MEGATOOLS_LICENSE = GPLv2

MEGATOOLS_CONF_OPTS = \
	--prefix=/usr

$(eval $(autotools-package))
