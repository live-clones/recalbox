################################################################################
#
# recalbox-romfs-imageviewer
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system imageviewer --extension '.jpg .jpeg .png .bmp .psd .tga .gif .hdr .pic .ppm .pgm .JPG .JPEG .PNG .BMP .PSD .TGA .GIF .HDR .PIC .PPM .PGM"' --fullname 'Screenshots' --platform ignore --theme imageviewer BR2_PACKAGE_RETROARCH

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


ifeq ($(BR2_PACKAGE_RETROARCH),y)
define CONFIGURE_IMAGEVIEWER
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_IMAGEVIEWER),Screenshots,$(SYSTEM_NAME_IMAGEVIEWER),.jpg .jpeg .png .bmp .psd .tga .gif .hdr .pic .ppm .pgm .JPG .JPEG .PNG .BMP .PSD .TGA .GIF .HDR .PIC .PPM .PGM",ignore,imageviewer,$(SOURCE_ROMDIR_IMAGEVIEWER),$(@D))
endef
RECALBOX_ROMFS_IMAGEVIEWER_CONFIGURE_CMDS += $(CONFIGURE_IMAGEVIEWER)
endif

$(eval $(generic-package))
