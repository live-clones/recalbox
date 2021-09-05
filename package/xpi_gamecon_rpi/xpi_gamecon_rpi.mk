################################################################################
#
# XPI_GAMECON_RPI Recalbox Team 2020
#
################################################################################

XPI_GAMECON_RPI_VERSION = v0.2
XPI_GAMECON_RPI_SITE = https://gitlab.com/recalbox/packages/hardware/xpi_gamecon_rpi.git
XPI_GAMECON_RPI_SITE_METHOD = git
XPI_GAMECON_RPI_DEPENDENCIES = linux
XPI_GAMECON_RPI_LICENSE = GPL-3.0

$(eval $(kernel-module))
$(eval $(generic-package))
