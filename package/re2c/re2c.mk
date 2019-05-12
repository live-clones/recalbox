################################################################################
#
# re2c
#
################################################################################

RE2C_VERSION = 1.1.1
RE2C_SITE = https://github.com/skvadrik/re2c/releases/download/$(RE2C_VERSION)
RE2C_LICENSE = PUBLIC DOMAIN
RE2C_LICENSE_FILES = LICENSE

$(eval $(host-autotools-package))
