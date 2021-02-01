################################################################################
#
# LIBXMP-LITE
#
################################################################################

LIBXMP_LITE_VERSION = 92313f6f525a8510a2492df4266abcf8f0b45834
LIBXMP_LITE_SITE = $(call github,cmatsuoka,libxmp,$(LIBXMP_LITE_VERSION))

LIBXMP_LITE_LICENSE = LGPL-2.1+
LIBXMP_LITE_LICENSE_FILES = README

$(eval $(generic-package))
