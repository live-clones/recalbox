################################################################################
#
# nvidia-driver version 460.80.02
#
################################################################################

NVIDIA_DRIVER_460_RECALBOX_VERSION = 460.84
NVIDIA_DRIVER_460_RECALBOX_SITE = http://download.nvidia.com/XFree86/Linux-x86_64/$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
NVIDIA_DRIVER_460_RECALBOX_SOURCE = NVIDIA-Linux-x86_64-$(NVIDIA_DRIVER_460_RECALBOX_VERSION).run
NVIDIA_DRIVER_460_RECALBOX_LICENSE = NVIDIA Software License
NVIDIA_DRIVER_460_RECALBOX_LICENSE_FILES = LICENSE
NVIDIA_DRIVER_460_RECALBOX_REDISTRIBUTE = NO
NVIDIA_DRIVER_460_RECALBOX_INSTALL_STAGING = YES

NVIDIA_DRIVER_460_RECALBOX_VERSION_GL = 1.7.0
NVIDIA_DRIVER_460_RECALBOX_VERSION_WAYLAND = 1.1.5

ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_XORG),y)

# Since nvidia-driver are binary blobs, the below dependencies are not
# strictly speaking build dependencies of nvidia-driver. However, they
# are build dependencies of packages that depend on nvidia-driver, so
# they should be built prior to those packages, and the only simple
# way to do so is to make nvidia-driver depend on them.
NVIDIA_DRIVER_460_RECALBOX_DEPENDENCIES = xlib_libX11 xlib_libXext

# recalbox - enable both mesa and nvidia
ifneq ($(BR2_PACKAGE_MESA3D),y)
NVIDIA_DRIVER_460_RECALBOX_PROVIDES = libgl libegl libgles
endif

# recalbox - enable both mesa and nvidia
ifneq ($(BR2_PACKAGE_MESA3D),y)
  NVIDIA_DRIVER_460_RECALBOX_DEPENDENCIES += mesa3d-headers
endif

# libGL.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) is the legacy libGL.so library; it
# has been replaced with libGL.so.1.0.0. Installing both is technically
# possible, but great care must be taken to ensure they do not conflict,
# so that EGL still works. The legacy library exposes an NVidia-specific
# API, so it should not be needed, except for legacy, binary-only
# applications (in other words: we don't care).
#
# libGL.so.1.0.0 is the new vendor-neutral library, aimed at replacing
# the old libGL.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) library. The latter contains
# NVidia extensions (which is deemed bad now), while the former follows
# the newly-introduced vendor-neutral "dispatching" API/ABI:
#   https://github.com/aritger/linux-opengl-abi-proposal/blob/master/linux-opengl-abi-proposal.txt
# However, this is not very usefull to us, as we don't support multiple
# GL providers at the same time on the system, which this proposal is
# aimed at supporting.
#
# So we only install the legacy library for now.
NVIDIA_DRIVER_460_RECALBOX_LIBS_GL = \
	libGLX.so.0 \
	libGL.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION_GL) \
	libGLX_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)

NVIDIA_DRIVER_460_RECALBOX_LIBS_EGL = \
	libEGL.so.1.1.0 \
	libGLdispatch.so.0 \
	libEGL_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)

NVIDIA_DRIVER_460_RECALBOX_LIBS_GLES = \
	libGLESv1_CM.so.1.2.0 \
	libGLESv2.so.2.1.0 \
	libGLESv1_CM_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libGLESv2_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)

NVIDIA_DRIVER_460_RECALBOX_LIBS_MISC = \
	libnvidia-eglcore.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-egl-wayland.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION_WAYLAND) \
	libnvidia-glcore.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-glsi.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-tls.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libvdpau_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-ml.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-gtk2.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-gtk3.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)

NVIDIA_DRIVER_460_RECALBOX_LIBS = \
	$(NVIDIA_DRIVER_460_RECALBOX_LIBS_GL) \
	$(NVIDIA_DRIVER_460_RECALBOX_LIBS_EGL) \
	$(NVIDIA_DRIVER_460_RECALBOX_LIBS_GLES) \
	$(NVIDIA_DRIVER_460_RECALBOX_LIBS_MISC)

# Install the gl.pc file
define NVIDIA_DRIVER_460_RECALBOX_INSTALL_GL_DEV
	$(INSTALL) -D -m 0644 package/nvidia-driver/gl.pc $(STAGING_DIR)/usr/lib/pkgconfig/gl.pc
	$(INSTALL) -D -m 0644 package/nvidia-driver/egl.pc $(STAGING_DIR)/usr/lib/pkgconfig/egl.pc
endef

# Those libraries are 'private' libraries requiring an agreement with
# NVidia to develop code for those libs. There seems to be no restriction
# on using those libraries (e.g. if the user has such an agreement, or
# wants to run a third-party program developped under such an agreement).
ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_PRIVATE_LIBS),y)
NVIDIA_DRIVER_460_RECALBOX_LIBS += \
	libnvidia-ifr.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-fbc.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
endif

# We refer to the destination path; the origin file has no directory component
NVIDIA_DRIVER_460_RECALBOX_X_MODS = \
	drivers/nvidia_drv.so \
	extensions/libglxserver_nvidia.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)

endif # X drivers

ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_CUDA),y)
NVIDIA_DRIVER_460_RECALBOX_LIBS += \
	libcuda.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-compiler.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvcuvid.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-ptxjitcompiler.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION) \
	libnvidia-encode.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_CUDA_PROGS),y)
NVIDIA_DRIVER_460_RECALBOX_PROGS = nvidia-cuda-mps-control nvidia-cuda-mps-server
endif
endif

ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_OPENCL),y)
NVIDIA_DRIVER_460_RECALBOX_LIBS += \
	libOpenCL.so.1.0.0 \
	libnvidia-opencl.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
endif

ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_NGX),y)
NVIDIA_DRIVER_460_RECALBOX_LIBS += \
	libnvidia-ngx.so.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
endif

# Build and install the kernel modules if needed
ifeq ($(BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_MODULE),y)

NVIDIA_DRIVER_460_RECALBOX_MODULES = nvidia nvidia-modeset nvidia-drm
ifeq ($(BR2_x86_64),y)
NVIDIA_DRIVER_460_RECALBOX_MODULES += nvidia-uvm
endif

# They can't do everything like everyone. They need those variables,
# because they don't recognise the usual variables set by the kernel
# build system. We also need to tell them what modules to build.
NVIDIA_DRIVER_460_RECALBOX_MODULE_MAKE_OPTS = \
	NV_KERNEL_SOURCES="$(LINUX_DIR)" \
	NV_KERNEL_OUTPUT="$(LINUX_DIR)" \
	NV_KERNEL_MODULES="$(NVIDIA_DRIVER_460_RECALBOX_MODULES)"

NVIDIA_DRIVER_460_RECALBOX_MODULE_SUBDIRS = kernel

$(eval $(kernel-module))

endif # BR2_PACKAGE_NVIDIA_DRIVER_460_RECALBOX_MODULE == y

# The downloaded archive is in fact an auto-extract script. So, it can run
# virtually everywhere, and it is fine enough to provide useful options.
# Except it can't extract into an existing (even empty) directory.
define NVIDIA_DRIVER_460_RECALBOX_EXTRACT_CMDS
	$(SHELL) $(NVIDIA_DRIVER_460_RECALBOX_DL_DIR)/$(NVIDIA_DRIVER_460_RECALBOX_SOURCE) --extract-only --target \
		$(@D)/tmp-extract
	chmod u+w -R $(@D)
	mv $(@D)/tmp-extract/* $(@D)/tmp-extract/.manifest $(@D)
	rm -rf $(@D)/tmp-extract
endef

# Helper to install libraries
# $1: destination directory (target or staging)
#
# For all libraries, we install them and create a symlink using
# their SONAME, so we can link to them at runtime; we also create
# the no-version symlink, so we can link to them at build time.
define NVIDIA_DRIVER_460_RECALBOX_INSTALL_LIBS
	$(foreach lib,$(NVIDIA_DRIVER_460_RECALBOX_LIBS),\
		$(INSTALL) -D -m 0755 $(@D)/$(lib) $(1)/usr/lib/extra/nvidia-$(NVIDIA_DRIVER_460_RECALBOX_VERSION)/$(notdir $(lib))
		libsoname="$$( $(TARGET_READELF) -d "$(@D)/$(lib)" \
			|sed -r -e '/.*\(SONAME\).*\[(.*)\]$$/!d; s//\1/;' )";)
endef

# For staging, install libraries and development files
define NVIDIA_DRIVER_460_RECALBOX_INSTALL_STAGING_CMDS
	$(call NVIDIA_DRIVER_460_RECALBOX_INSTALL_LIBS,$(STAGING_DIR))
	$(NVIDIA_DRIVER_460_RECALBOX_INSTALL_GL_DEV)
endef

# For target, install libraries and X.org modules
define NVIDIA_DRIVER_460_RECALBOX_INSTALL_TARGET_CMDS
	$(call NVIDIA_DRIVER_460_RECALBOX_INSTALL_LIBS,$(TARGET_DIR))
	$(foreach m,$(NVIDIA_DRIVER_460_RECALBOX_X_MODS), \
		$(INSTALL) -D -m 0755 $(@D)/$(notdir $(m)) \
			$(TARGET_DIR)/usr/lib/extra/nvidia-$(NVIDIA_DRIVER_460_RECALBOX_VERSION)/xorg/$(m)
	)
	$(foreach p,$(NVIDIA_DRIVER_460_RECALBOX_PROGS), \
		$(INSTALL) -D -m 0755 $(@D)/$(p) \
			$(TARGET_DIR)/usr/bin/$(p).$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	)
	$(NVIDIA_DRIVER_460_RECALBOX_INSTALL_KERNEL_MODULE)

	# recalbox install more needed
	$(INSTALL) -D -m 0644 $(@D)/10_nvidia.json $(TARGET_DIR)/usr/share/glvnd/egl_vendor.d/10_nvidia.json.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	$(INSTALL) -D -m 0644 $(@D)/10_nvidia_wayland.json $(TARGET_DIR)/usr/share/glvnd/egl_vendor.d/10_nvidia_wayland.json.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	$(INSTALL) -D -m 0644 $(@D)/nvidia-drm-outputclass.conf $(TARGET_DIR)/usr/share/X11/xorg.conf.d/10-nvidia-drm.conf.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	$(INSTALL) -D -m 0755 $(@D)/nvidia-settings $(TARGET_DIR)/usr/bin/nvidia-settings.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	$(INSTALL) -D -m 0755 $(@D)/nvidia-smi $(TARGET_DIR)/usr/bin/nvidia-smi.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
endef

# recalbox hardware script will handle kernel module version
define NVIDIA_DRIVER_460_RECALBOX_POST_INSTALL_TARGET_KERNEL_MODULES
	mv $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-drm.ko $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-drm.ko.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	mv $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-modeset.ko $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-modeset.ko.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	mv $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-uvm.ko $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia-uvm.ko.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
	mv $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia.ko $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/extra/nvidia.ko.$(NVIDIA_DRIVER_460_RECALBOX_VERSION)
endef

NVIDIA_DRIVER_460_RECALBOX_POST_INSTALL_TARGET_HOOKS += NVIDIA_DRIVER_460_RECALBOX_POST_INSTALL_TARGET_KERNEL_MODULES

$(eval $(generic-package))
