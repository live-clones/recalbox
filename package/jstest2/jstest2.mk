################################################################################
#
# jstest2
#
################################################################################

JSTEST2_VERSION = aafbdb1ed3e687583037ba55ae88b1210d6ce98b
JSTEST2_SITE = https://gitlab.com/sdl-jstest/sdl-jstest.git
JSTEST2_SITE_METHOD = git
JSTEST2_GIT_SUBMODULES = y
JSTEST2_DEPENDENCIES = sdl sdl2
JSTEST2_LICENSE = GPL-3.0
JSTEST2_LICENSE_FILES = LICENSE.txt

$(eval $(cmake-package))
