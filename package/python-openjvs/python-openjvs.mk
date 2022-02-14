################################################################################
#
# py-openjvs
#
################################################################################

PYTHON_OPENJVS_VERSION = 2.0.1
PYTHON_OPENJVS_SOURCE = py-openjvs-$(PYTHON_OPENJVS_VERSION).tar.gz
PYTHON_OPENJVS_SITE = https://files.pythonhosted.org/packages/ca/92/2c7265581938b5d5c0f887ef5be94f0c2c4eb1041e8ba7a9f06d92fb83d6
PYTHON_OPENJVS_LICENSE = Apache-2.0 (library), GPL-3.0+ (test, build)
PYTHON_OPENJVS_LICENSE_FILES = LICENSE.ASF-2 LICENSE.GPL-3
PYTHON_OPENJVS_SETUP_TYPE = setuptools
$(eval $(python-package))
