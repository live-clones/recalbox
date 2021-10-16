################################################################################
#
# py-uinput
#
################################################################################

PYTHON_UINPUT_VERSION = 0.11.2
PYTHON_UINPUT_SOURCE = python-uinput-$(PYTHON_UINPUT_VERSION).tar.gz
PYTHON_UINPUT_SITE = https://files.pythonhosted.org/packages/54/b7/be7d0e8bbbbd440fef31242974d92d4edd21eb95ed96078b18cf207c7ccb
PYTHON_UINPUT_LICENSE = Apache-2.0 (library), GPL-3.0+ (test, build)
PYTHON_UINPUT_LICENSE_FILES = LICENSE.ASF-2 LICENSE.GPL-3
PYTHON_UINPUT_SETUP_TYPE = distutils
$(eval $(python-package))
