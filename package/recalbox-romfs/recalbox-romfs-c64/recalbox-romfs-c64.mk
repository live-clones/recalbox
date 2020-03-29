################################################################################
#
# recalbox-romfs-c64
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system c64 --extension '.d64 .D64 .d71 .D71 .d80 .D80 .d81 .D81 .d82 .D82 .g64 .G64 .g41 .G41 .x64 .X64 .t64 .T64 .tap .TAP .prg .PRG .p00 .P00 .crt .CRT .bin .BIN .gz .GZ .d6z .D6Z .d7z .D7Z .d8z .D8Z .g6z .G6Z .g4z .G4Z .x6z .X6Z .cmd .CMD .m3u .M3U .zip .ZIP .7z .7Z' --fullname 'Commodore 64' --platform c64 --theme c64 1:libretro:vice_x64sc:BR2_PACKAGE_LIBRETRO_VICE 2:libretro:vice_x64:BR2_PACKAGE_LIBRETRO_VICE 3:libretro:vice_xplus4:BR2_PACKAGE_LIBRETRO_VICE 4:libretro:vice_xpet:BR2_PACKAGE_LIBRETRO_VICE 5:libretro:vice_xvic:BR2_PACKAGE_LIBRETRO_VICE 6:libretro:vice_x128:BR2_PACKAGE_LIBRETRO_VICE 7:libretro:vice_xcbm2:BR2_PACKAGE_LIBRETRO_VICE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_C64_SOURCE = 
RECALBOX_ROMFS_C64_SITE = 
RECALBOX_ROMFS_C64_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_C64 = c64
SYSTEM_XML_C64 = $(@D)/$(SYSTEM_NAME_C64).xml
# System rom path
SOURCE_ROMDIR_C64 = $(RECALBOX_ROMFS_C64_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VICE),)
define CONFIGURE_MAIN_C64_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_C64),Commodore 64,$(SYSTEM_NAME_C64),.d64 .D64 .d71 .D71 .d80 .D80 .d81 .D81 .d82 .D82 .g64 .G64 .g41 .G41 .x64 .X64 .t64 .T64 .tap .TAP .prg .PRG .p00 .P00 .crt .CRT .bin .BIN .gz .GZ .d6z .D6Z .d7z .D7Z .d8z .D8Z .g6z .G6Z .g4z .G4Z .x6z .X6Z .cmd .CMD .m3u .M3U .zip .ZIP .7z .7Z,c64,c64)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VICE),)
define CONFIGURE_C64_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_C64),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_XPET_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_xpet,4)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_X128_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_x128,6)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_X64_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_x64,2)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_XVIC_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_xvic,5)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_X64SC_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_x64sc,1)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_XPLUS4_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_xplus4,3)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_VICE),y)
define CONFIGURE_C64_LIBRETRO_VICE_XCBM2_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),vice_xcbm2,7)
endef
endif

define CONFIGURE_C64_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_C64))
endef
endif



define CONFIGURE_MAIN_C64_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_C64),$(SOURCE_ROMDIR_C64),$(@D))
endef
endif

define RECALBOX_ROMFS_C64_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_C64_START)
	$(CONFIGURE_C64_LIBRETRO_START)
	$(CONFIGURE_C64_LIBRETRO_VICE_XPET_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_X128_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_X64_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_XVIC_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_X64SC_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_XPLUS4_DEF)
	$(CONFIGURE_C64_LIBRETRO_VICE_XCBM2_DEF)
	$(CONFIGURE_C64_LIBRETRO_END)
	$(CONFIGURE_MAIN_C64_END)
endef

$(eval $(generic-package))
