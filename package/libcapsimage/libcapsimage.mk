################################################################################
#
# LIBCAPSIMAGE
#
################################################################################

LIBCAPSIMAGE_SOURCE = spsdeclib_$(LIBCAPSIMAGE_VERSION)_source.zip
LIBCAPSIMAGE_VERSION = 5.1
LIBCAPSIMAGE_SITE = http://www.kryoflux.com/download
LIBCAPSIMAGE_LICENSE = COPYRIGHT
LIBCAPSIMAGE_NON_COMMERCIAL = y

LIBCAPSIMAGE_SITE_METHOD = wget
LIBCAPSIMAGE_SUBDIR = capsimg_source_linux_macosx/CAPSImg

LIBCAPSIMAGE_INSTALL_STAGING = YES

define LIBCAPSIMAGE_EXTRACT_CMDS
	$(UNZIP) $(DL_DIR)/libcapsimage/$(LIBCAPSIMAGE_SOURCE) -d $(@D)
	$(UNZIP) -x $(@D)/capsimg_source_linux_macosx.zip  -d $(@D)
endef

define LIBCAPSIMAGE_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)/usr/include/caps
	cp $(@D)/$(LIBCAPSIMAGE_SUBDIR)/libcapsimage.so.$(LIBCAPSIMAGE_VERSION) $(STAGING_DIR)/usr/lib
	cp -r $(@D)/capsimg_source_linux_macosx/LibIPF/*.h $(STAGING_DIR)/usr/include/caps/
	cp -r $(@D)/capsimg_source_linux_macosx/Core/*.h $(STAGING_DIR)/usr/include/caps/
	ln -sf $(STAGING_DIR)/usr/lib/libcapsimage.so.$(LIBCAPSIMAGE_VERSION) $(STAGING_DIR)/usr/lib/libcapsimage.so
endef

define LIBCAPSIMAGE_INSTALL_TARGET_CMDS
	cp $(@D)/$(LIBCAPSIMAGE_SUBDIR)/libcapsimage.so.$(LIBCAPSIMAGE_VERSION) $(TARGET_DIR)/usr/lib
	ln -sf /usr/lib/libcapsimage.so.$(LIBCAPSIMAGE_VERSION) $(TARGET_DIR)/usr/lib/libcapsimage.so.5
	ln -sf /usr/lib/libcapsimage.so.$(LIBCAPSIMAGE_VERSION) $(TARGET_DIR)/usr/lib/libcapsimage.so
endef

define LIBCAPSIMAGE_PRE_CONFIGURE_FIXUP
	chmod u+x $(@D)/$(LIBCAPSIMAGE_SUBDIR)/configure
endef

LIBCAPSIMAGE_PRE_CONFIGURE_HOOKS += LIBCAPSIMAGE_PRE_CONFIGURE_FIXUP

$(eval $(autotools-package))
