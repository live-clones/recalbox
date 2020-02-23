################################################################################
#
# recalbox-romfs-imageviewer
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system imageviewer --extension '.jpg .jpeg .png .bmp .psd .tga .gif .hdr .pic .ppm .pgm .JPG .JPEG .PNG .BMP .PSD .TGA .GIF .HDR .PIC .PPM .PGM"' --fullname 'Screenshots' --platform ignore --theme imageviewer 1:imageviewer:imageviewer:BR2_PACKAGE_RETROARCH

# Name the 3 vars as the package requires
RECALBOX_ROMFS_IMAGEVIEWER_SOURCE = 
RECALBOX_ROMFS_IMAGEVIEWER_SITE = 
RECALBOX_ROMFS_IMAGEVIEWER_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_IMAGEVIEWER = imageviewer
SYSTEM_XML_IMAGEVIEWER = $(@D)/$(SYSTEM_NAME_IMAGEVIEWER).xml
# System rom path
SOURCE_ROMDIR_IMAGEVIEWER = $(RECALBOX_ROMFS_IMAGEVIEWER_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_RETROARCH),)
define CONFIGURE_MAIN_IMAGEVIEWER_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_IMAGEVIEWER),Screenshots,$(SYSTEM_NAME_IMAGEVIEWER),.jpg .jpeg .png .bmp .psd .tga .gif .hdr .pic .ppm .pgm .JPG .JPEG .PNG .BMP .PSD .TGA .GIF .HDR .PIC .PPM .PGM",ignore,imageviewer)
endef

ifneq ($(BR2_PACKAGE_RETROARCH),)
define CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_IMAGEVIEWER),imageviewer)
endef
ifeq ($(BR2_PACKAGE_RETROARCH),y)
define CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_IMAGEVIEWER_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_IMAGEVIEWER),imageviewer,1)
endef
endif

define CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_IMAGEVIEWER))
endef
endif



define CONFIGURE_MAIN_IMAGEVIEWER_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_IMAGEVIEWER),$(SOURCE_ROMDIR_IMAGEVIEWER),$(@D))
endef
endif

define RECALBOX_ROMFS_IMAGEVIEWER_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_IMAGEVIEWER_START)
	$(CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_START)
	$(CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_IMAGEVIEWER_DEF)
	$(CONFIGURE_IMAGEVIEWER_IMAGEVIEWER_END)
	$(CONFIGURE_MAIN_IMAGEVIEWER_END)
endef

$(eval $(generic-package))
