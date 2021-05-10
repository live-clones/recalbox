################################################################################
#
# recalbox-romfs
#
################################################################################

RECALBOX_ROMFS_SOURCE =
RECALBOX_ROMFS_SITE =
RECALBOX_ROMFS_INSTALL_STAGING = NO

ES_SYSTEMS = $(@D)/es_systems.cfg
ES_SYSTEMS_TMP = $(ES_SYSTEMS).tmp
TARGET_ES_SYSTEMS_DIR = $(TARGET_DIR)/recalbox/share_init/system/.emulationstation
TARGET_ROMDIR = $(TARGET_DIR)/recalbox/share_init/

CONFIGGEN_STD_CMD = python /usr/lib/python2.7/site-packages/configgen/emulatorlauncher.pyc %CONTROLLERSCONFIG% -system %SYSTEM% -rom %ROM% -emulator %EMULATOR% -core %CORE% -ratio %RATIO% %NETPLAY%

# Each emulator must define:
#  - its es_systems.cfg entry
#  - its source roms folder
# You MUST add an empty line at the end of the define otherwise some commands
# will overlap and fail

# function to add the new system
# $1 = XML file
# $2 = full system name
# $3 = short system name
# $4 = e=list of extensions ex : .zip .ZIP
# $5 = platform
# $6 = theme
# $7 = extra configgen command line args
define RECALBOX_ROMFS_CALL_ADD_SYSTEM
    echo -e '<system>\n' \
    '<fullname>$(2)</fullname>\n' \
    "<name>$(3)</name>\n" \
    '<path>/recalbox/share/roms/$(3)</path>\n' \
    '<extension>$(4)</extension>\n' \
    "<command>$(CONFIGGEN_STD_CMD)$(7)</command>\n" \
    '<platform>$(5)</platform>\n' \
    '<theme>$(6)</theme>\n' \
    '<emulators>' > $(1)
endef

# function to add the new system
# $1 = XML file
# $2 = full system name
# $3 = short system name
# $4 = e=list of extensions ex : .zip .ZIP
# $5 = platform
# $6 = theme
# $7 = full rom path
# $8 = extra configgen command line args
define RECALBOX_ROMFS_CALL_ADD_SYSTEM_ROMPATH
    echo -e '<system>\n' \
    '<fullname>$(2)</fullname>\n' \
    "<name>$(3)</name>\n" \
    '<path>$(7)</path>\n' \
    '<extension>$(4)</extension>\n' \
    "<command>$(CONFIGGEN_STD_CMD)$(8)</command>\n" \
    '<platform>$(5)</platform>\n' \
    '<theme>$(6)</theme>\n' \
    '<emulators>' > $(1)
endef

# function to add the new port
# $1 = XML file
# $2 = full system name
# $3 = short system name
# $4 = e=list of extensions ex : .zip .ZIP
# $5 = platform
# $6 = theme
# $7 = read only
# $8 = extra configgen command line args
define RECALBOX_ROMFS_CALL_ADD_PORT
    echo -e '<system>\n' \
    '<fullname>$(2)</fullname>\n' \
    "<name>$(3)</name>\n" \
    '<path readonly="$(7)">/recalbox/share/roms/ports/$(2)</path>\n' \
    '<extension>$(4)</extension>\n' \
    "<command>$(CONFIGGEN_STD_CMD)$(8)</command>\n" \
    '<platform>$(5)</platform>\n' \
    '<theme>$(6)</theme>\n' \
    '<emulators>' > $(1)
    sed -i -e 's/\&/\&amp;/g' $(1)
endef

# function to add the emulator part of a XML
# $1 = XML file
# $2 = emulator name name
RECALBOX_ROMFS_CALL_START_EMULATOR = echo -e '<emulator name="$(2)">\n<cores>' >> $(1)

# function to add the core part of a XML
# $1 = XML file
# $2 = core name
RECALBOX_ROMFS_CALL_ADD_CORE = echo -e '<core priority="$(3)">$(2)</core>' >> $(1)

# function to close the emulator part of a XML
# $1 = XML file
RECALBOX_ROMFS_CALL_END_EMULATOR = echo -e '</cores>\n</emulator>' >> $(1)

# function to add a standlone emulator part of a XML
# $1 = XML file
# $2 = emulator name name
RECALBOX_ROMFS_CALL_STANDALONE_EMULATOR = echo -e '<emulator name="$(2)"/>' >> $(1)

# function to add the new system
# $1 = XML file
# $2 = system rom source dir
# $3 = system rom destination dir
define RECALBOX_ROMFS_CALL_END_SYSTEM
    echo -e '</emulators>\n</system>' >> $(1)
    cp -R $(2) $(3)
endef

# function to add the new system
# $1 = XML file
# $2 = system rom source dir
# $3 = system rom destination dir
# $4 = system name
define RECALBOX_ROMFS_CALL_END_PORT
    echo -e '</emulators>\n</system>' >> $(1)
    cp -R $(2) $(3)
    #mv $(3)/roms/ports/gamelist.xml $(3)/roms/ports/$(4)_gamelist.xml
endef

# function to add a new system that only has a standalone emulator
# $1 = XML file
# $2 = full system name
# $3 = short system name
# $4 = e=list of extensions ex : .zip .ZIP
# $5 = platform
# $6 = theme
# $7 = system rom source dir
# $8 = system rom destination dir
# $9 = extra configgen command line args
RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM = $(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM_FULLPATH,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),/recalbox/share/roms/$(3),$(9))

# function to add a new system that only has a standalone emulator specifying
# its full path to the roms folder
# $1 = XML file
# $2 = full system name
# $3 = short system name
# $4 = e=list of extensions ex : .zip .ZIP
# $5 = platform
# $6 = theme
# $7 = system rom source dir
# $8 = system rom destination dir
# $9 = full path to roms
# $10 = extra configgen command line args
define RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM_FULLPATH
    echo -e '<system>\n' \
    '<fullname>$(2)</fullname>\n' \
    "<name>$(3)</name>\n" \
    '<path>$(9)</path>\n' \
    '<extension>$(4)</extension>\n' \
    "<command>$(CONFIGGEN_STD_CMD)$(10)</command>\n" \
    '<platform>$(5)</platform>\n' \
    '<theme>$(6)</theme>\n' \
    '<emulators />\n' \
    '</system>' >> $(1)
    cp -R $(7) $(8)
endef

# Init the es_systems.cfg
# Keep the empty line as there are several commands to chain at configure
define RECALBOX_ROMFS_ES_SYSTEMS
	echo '<?xml version="1.0"?>' > $(ES_SYSTEMS_TMP)
	echo '<systemList>' >>  $(ES_SYSTEMS_TMP)
	cat $(@D)/../recalbox-romfs-*/*.xml >> $(ES_SYSTEMS_TMP)
	echo -e '<system>\n' \
		"\t<fullname>Favorites</fullname>\n" \
		'\t<name>favorites</name>\n' \
		"\t<command>$(CONFIGGEN_STD_CMD)</command>\n" \
		"\t<theme>favorites</theme>\n" \
	"</system>\n" \
	'</systemList>' >>  $(ES_SYSTEMS_TMP)
	xmllint --format $(ES_SYSTEMS_TMP) > $(ES_SYSTEMS)

endef
RECALBOX_ROMFS_CONFIGURE_CMDS += $(RECALBOX_ROMFS_ES_SYSTEMS)

# Copy rom dirs
define RECALBOX_ROMFS_ROM_DIRS
	cp -R $(@D)/../recalbox-romfs-*/roms $(@D)
    #python2 $(BR2_EXTERNAL_RECALBOX_PATH)/scripts/linux/merge_gamelists.py -i $(@D)/roms/ports -o $(@D)/roms/ports/gamelist.xml

endef
RECALBOX_ROMFS_CONFIGURE_CMDS += $(RECALBOX_ROMFS_ROM_DIRS)

# Copy from build to target
define RECALBOX_ROMFS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_ROMDIR)/roms
	mkdir -p $(TARGET_ES_SYSTEMS_DIR)
	$(INSTALL) -D -m 0644 $(ES_SYSTEMS) $(TARGET_ES_SYSTEMS_DIR)
	cp -r $(@D)/roms $(TARGET_ROMDIR)
endef

# System: amigacd32
ifneq ($(BR2_PACKAGE_LIBRETRO_UAE)$(BR2_PACKAGE_AMIBERRY),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-amigacd32
endif

# System: dungeoncrawlstonesoup
ifneq ($(BR2_PACKAGE_LIBRETRO_CRAWL),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-dungeoncrawlstonesoup
endif

# System: gx4000
ifneq ($(BR2_PACKAGE_LIBRETRO_CAP32),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-gx4000
endif

# System: o2em
ifneq ($(BR2_PACKAGE_LIBRETRO_O2EM),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-o2em
endif

# System: pcv2
ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-pcv2
endif

# System: samcoupe
ifeq ($(BR2_PACKAGE_SIMCOUPE),y)
	RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-samcoupe
endif

# System: thepowdertoy
ifneq ($(BR2_PACKAGE_LIBRETRO_THEPOWDERTOY),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-thepowdertoy
endif

# System: uzebox
ifneq ($(BR2_PACKAGE_LIBRETRO_UZEM),)
    RECALBOX_ROMFS_DEPENDENCIES += recalbox-romfs-uzebox
endif

$(eval $(generic-package))
