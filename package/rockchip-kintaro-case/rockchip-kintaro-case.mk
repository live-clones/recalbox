################################################################################
#
# Kintaro-Case Script for ROCK64/ROCKPRO
#
################################################################################

ROCKCHIP_KINTARO_CASE_SITE = $(call github,mrfixit2001,KintaroSnes-Rock64,$(ROCKCHIP_KINTARO_CASE_VERSION))
ROCKCHIP_KINTARO_CASE_VERSION = e5709fd9fb5e74b12e2c5f4f065aa63bbc8b83fb
ROCKCHIP_KINTARO_CASE_DEPENDENCIES = R64GPIO

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RK3399),y)
  define ROCKCHIP_KINTARO_CASE_POST_INSTALL
	sed -i 's/GPIO.setrock("ROCK64")/GPIO.setrock("ROCKPRO64")/g' $(TARGET_DIR)/recalbox/scripts/kintaro-case.py
  endef
  ROCKCHIP_KINTARO_CASE_POST_INSTALL_TARGET_HOOKS += ROCKCHIP_KINTARO_CASE_POST_INSTALL
endif

define ROCKCHIP_KINTARO_CASE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/scripts
	$(INSTALL) -m 755 $(@D)/recalbox/scripts/* $(TARGET_DIR)/recalbox/scripts

	mkdir -p $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 $(@D)/etc/init.d/* $(TARGET_DIR)/etc/init.d
endef

$(eval $(generic-package))
