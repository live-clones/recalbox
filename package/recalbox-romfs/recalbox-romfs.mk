################################################################################
#
# recalbox-romfs
#
################################################################################

RECALBOX_ROMFS_LICENSE = RECALBOX

include $(sort $(wildcard $(BR2_EXTERNAL_RECALBOX_PATH)/package/recalbox-romfs/*/*.mk))
