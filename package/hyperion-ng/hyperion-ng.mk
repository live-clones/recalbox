################################################################################
#
# Hyperion-ng
#
################################################################################

HYPERION_NG_SITE = git://github.com/hyperion-project/hyperion.ng.git
HYPERION_NG_VERSION = 6d004240ded5399b5d747a8e9bef7f1888a452c9
HYPERION_NG_LICENSE = MIT
HYPERION_NG_SITE_METHOD = git
HYPERION_NG_GIT_SUBMODULES = YES

# I was adding dependencies one by one, python totally blocked me
HYPERION_NG_DEPENDENCIES += python3 qt5serialport

# Real conf for hyperion
HYPERION_NG_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release -DENABLE_QT=OFF

# -------------------------
# Below is tests for python3
# Just in case
#HYPERION_NG_CONF_OPTS += -DPython_ADDITIONAL_VERSIONS=3.9
# The variables that cmake do not find can be overloaded in the command 
# This line is taken in account (i don't know yet if it's the host, the staging or the target we should use)
HYPERION_NG_CONF_OPTS += -DPython3_INCLUDE_DIR="$(HOST_DIR)/arm-buildroot-linux-gnueabihf/sysroot/usr/include/python3.9/"

# Those line is taken in account if you add the policy CMAKE_POLICY( SET CMP0074 NEW )
# But it changes nothing
# see https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure
HYPERION_NG_CONF_OPTS += -DPython3_ROOT_DIR="$(PYTHON3_PATH)"
HYPERION_NG_CONF_OPTS += -DPython3_ROOT="$(PYTHON3_PATH)"
HYPERION_NG_CONF_OPTS += -DCMAKE_PREFIX_PATH="$(STAGING_DIR)"

# The library i cannot fix
HYPERION_NG_CONF_OPTS += -DPython3_LIBRARY="$(HOST_DIR)/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/python3.9/config-3.9-arm-linux-gnueabihf/libpython3.9.a"

# And i tried to force the PYTHON_EXECUTABLE
HYPERION_NG_CONF_OPTS += -DPYTHON_EXECUTABLE="$(HOST_DIR)/usr/bin/python3"
HYPERION_NG_CONF_OPTS += -DPYTHON3_EXECUTABLE="$(HOST_DIR)/usr/bin/python3"
$(eval $(cmake-package))
