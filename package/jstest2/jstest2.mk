################################################################################
#
# jstest2
#
################################################################################

JSTEST2_VERSION = aafbdb1ed3e687583037ba55ae88b1210d6ce98b
JSTEST2_SITE = $(call github,Grumbel,sdl-jstest,$(JSTEST2_VERSION))
JSTEST2_DEPENDENCIES = sdl sdl2

$(eval $(cmake-package))
