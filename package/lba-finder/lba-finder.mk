################################################################################
#
# xz
#
################################################################################

LBA_FINDER_VERSION = v1.0
LBA_FINDER_SITE = https://gitlab.com/recalbox/lba-finder
LBA_FINDER_SITE_METHOD = git
LBA_FINDER_LICENSE = GPL-2.0+

$(eval $(autotools-package))
