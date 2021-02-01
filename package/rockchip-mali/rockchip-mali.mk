################################################################################
#
# mali-t62x
#
################################################################################

ROCKCHIP_MALI_VERSION = 239b3157148cfc34b10e664b109b4420ddf7b54c
ROCKCHIP_MALI_SITE = $(call github,rockchip-linux,libmali,$(ROCKCHIP_MALI_VERSION))
ROCKCHIP_MALI_INSTALL_STAGING = YES

ROCKCHIP_MALI_PROVIDES = libegl libgles
ROCKCHIP_MALI_DEPENDENCIES = libdrm

ROCKCHIP_MALI_LICENSE = COPYRIGHT
ROCKCHIP_MALI_LICENSE_FILES = END_USER_LICENCE_AGREEMENT.txt

ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_BIFROST),y)
# bifrost
ROCKCHIP_MALI_REVISION = rxp0
ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_BIFROST_GBM),y)
ROCKCHIP_MALI_LIB = libmali-bifrost-g31-$(ROCKCHIP_MALI_REVISION)-gbm.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_BIFROST_WAYLAND),y)
ROCKCHIP_MALI_LIB = libmali-bifrost-g31-$(ROCKCHIP_MALI_REVISION)-wayland.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_BIFROST_X11),y)
ROCKCHIP_MALI_LIB = libmali-bifrost-g31-$(ROCKCHIP_MALI_REVISION)-x11.so
endif
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_MIDGARD),y)
# midgard
ROCKCHIP_MALI_REVISION = r18p0
ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_MIDGARD_GBM),y)
ROCKCHIP_MALI_LIB = libmali-midgard-t86x-$(ROCKCHIP_MALI_REVISION)-gbm.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_MIDGARD_WAYLAND),y)
ROCKCHIP_MALI_LIB = libmali-midgard-t86x-$(ROCKCHIP_MALI_REVISION)-wayland.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_MIDGARD_X11),y)
ROCKCHIP_MALI_LIB = libmali-midgard-t86x-$(ROCKCHIP_MALI_REVISION)-x11.so
endif
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_UTGARD),y)
# utgard
ROCKCHIP_MALI_REVISION = r7p0
ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_UTGARD_GBM),y)
ROCKCHIP_MALI_LIB = libmali-utgard-450-$(ROCKCHIP_MALI_REVISION)-gbm.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_UTGARD_WAYLAND),y)
ROCKCHIP_MALI_LIB = libmali-utgard-450-$(ROCKCHIP_MALI_REVISION)-wayland.so
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_UTGARD_X11),y)
ROCKCHIP_MALI_LIB = libmali-utgard-450-$(ROCKCHIP_MALI_REVISION)-x11.so
endif
endif

ifeq ($(BR2_arm),y)
ROCKCHIP_MALI_ARCH = arm-linux-gnueabihf
endif
ifeq ($(BR2_aarch64),y)
ROCKCHIP_MALI_ARCH = aarch64-linux-gnu
endif

define ROCKCHIP_MALI_INSTALL_CMDS
	$(INSTALL) -D $(@D)/lib/$(ROCKCHIP_MALI_ARCH)/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/$(ROCKCHIP_MALI_LIB)
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libEGL.so
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libEGL.so.1
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libEGL.so.1.0.0
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libgbm.so
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libgbm.so.1
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libgbm.so.1.0.0
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv1_CM.so
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv1_CM.so.1
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv1_CM.so.1.0.0
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv2.so
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv2.so.2
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libGLESv2.so.2.0.0
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libmali.so
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libmali.so.1
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libmali.so.1.0.0
	ln -rsf $(1)/usr/lib/$(ROCKCHIP_MALI_LIB) $(1)/usr/lib/libOpenCL.so
	$(INSTALL) -D $(@D)/include/gbm.h                              $(1)/usr/include/gbm.h
	$(INSTALL) -D $(@D)/include/CL/cl_ext_intel.h                  $(1)/usr/include/CL/cl_ext_intel.h
	$(INSTALL) -D $(@D)/include/CL/cl_platform.h                   $(1)/usr/include/CL/cl_platform.h
	$(INSTALL) -D $(@D)/include/CL/cl.h                            $(1)/usr/include/CL/cl.h
	$(INSTALL) -D $(@D)/include/CL/cl_dx9_media_sharing_intel.h    $(1)/usr/include/CL/cl_dx9_media_sharing_intel.h
	$(INSTALL) -D $(@D)/include/CL/cl_gl_ext.h                     $(1)/usr/include/CL/cl_gl_ext.h
	$(INSTALL) -D $(@D)/include/CL/cl_egl.h                        $(1)/usr/include/CL/cl_egl.h
	$(INSTALL) -D $(@D)/include/CL/cl_d3d11.h                      $(1)/usr/include/CL/cl_d3d11.h
	$(INSTALL) -D $(@D)/include/CL/cl_d3d10.h                      $(1)/usr/include/CL/cl_d3d10.h
	$(INSTALL) -D $(@D)/include/CL/cl_va_api_media_sharing_intel.h $(1)/usr/include/CL/cl_va_api_media_sharing_intel.h
	$(INSTALL) -D $(@D)/include/CL/cl_gl.h                         $(1)/usr/include/CL/cl_gl.h
	$(INSTALL) -D $(@D)/include/CL/cl_dx9_media_sharing.h          $(1)/usr/include/CL/cl_dx9_media_sharing.h
	$(INSTALL) -D $(@D)/include/CL/cl_ext.h                        $(1)/usr/include/CL/cl_ext.h
	$(INSTALL) -D $(@D)/include/CL/opencl.h                        $(1)/usr/include/CL/opencl.h
	$(INSTALL) -D $(@D)/include/GLES3/gl3.h                        $(1)/usr/include/GLES3/gl3.h
	$(INSTALL) -D $(@D)/include/GLES3/gl31.h                       $(1)/usr/include/GLES3/gl31.h
	$(INSTALL) -D $(@D)/include/GLES3/gl32.h                       $(1)/usr/include/GLES3/gl32.h
	$(INSTALL) -D $(@D)/include/GLES3/gl3platform.h                $(1)/usr/include/GLES3/gl3platform.h
	$(INSTALL) -D $(@D)/include/GLES2/gl2.h                        $(1)/usr/include/GLES2/gl2.h
	$(INSTALL) -D $(@D)/include/GLES2/gl2ext.h                     $(1)/usr/include/GLES2/gl2ext.h
	$(INSTALL) -D $(@D)/include/GLES2/gl2platform.h                $(1)/usr/include/GLES2/gl2platform.h
	$(INSTALL) -D $(@D)/include/EGL/eglplatform.h                  $(1)/usr/include/EGL/eglplatform.h
	$(INSTALL) -D $(@D)/include/EGL/eglext.h                       $(1)/usr/include/EGL/eglext.h
	$(INSTALL) -D $(@D)/include/EGL/egl.h                          $(1)/usr/include/EGL/egl.h
	$(INSTALL) -D $(@D)/include/GLES/gl.h                          $(1)/usr/include/GLES/gl.h
	$(INSTALL) -D $(@D)/include/GLES/glplatform.h                  $(1)/usr/include/GLES/glplatform.h
	$(INSTALL) -D $(@D)/include/GLES/glext.h                       $(1)/usr/include/GLES/glext.h
	$(INSTALL) -D $(@D)/include/GLES/egl.h                         $(1)/usr/include/GLES/egl.h
	$(INSTALL) -D $(@D)/include/KHR/mali_khrplatform.h             $(1)/usr/include/KHR/mali_khrplatform.h
	$(INSTALL) -D $(@D)/include/FBDEV/eglplatform.h                $(1)/usr/include/FBDEV/eglplatform.h
	$(INSTALL) -D $(@D)/include/FBDEV/eglext.h                     $(1)/usr/include/FBDEV/eglext.h
	$(INSTALL) -D $(@D)/include/FBDEV/mali_fbdev_types.h           $(1)/usr/include/FBDEV/mali_fbdev_types.h
	$(INSTALL) -D $(@D)/include/FBDEV/egl.h                        $(1)/usr/include/FBDEV/egl.h
	$(INSTALL) -D $(ROCKCHIP_MALI_PKGDIR)/egl.pc $(1)/usr/lib/pkgconfig/egl.pc
	$(INSTALL) -D $(ROCKCHIP_MALI_PKGDIR)/gbm.pc $(1)/usr/lib/pkgconfig/gbm.pc
	$(INSTALL) -D $(ROCKCHIP_MALI_PKGDIR)/glesv2.pc $(1)/usr/lib/pkgconfig/glesv2.pc
	$(INSTALL) -D $(ROCKCHIP_MALI_PKGDIR)/mali.pc $(1)/usr/lib/pkgconfig/mali.pc
	$(INSTALL) -D $(ROCKCHIP_MALI_PKGDIR)/OpenCL.pc $(1)/usr/lib/pkgconfig/OpenCL.pc
	ln -rsf $(1)/usr/include/KHR/mali_khrplatform.h $(1)/usr/include/KHR/khrplatform.h
endef

define ROCKCHIP_MALI_INSTALL_TARGET_CMDS
	$(call ROCKCHIP_MALI_INSTALL_CMDS,$(TARGET_DIR))
endef

define ROCKCHIP_MALI_INSTALL_STAGING_CMDS
	$(call ROCKCHIP_MALI_INSTALL_CMDS,$(STAGING_DIR))
endef

$(eval $(generic-package))
