=======================================================================
# RECALBOX: System $(FULLNAME) ($(TYPE:Arcade system:Home console:Handheld console:Computer:Fantasy console:Game engine:Port))
=======================================================================

Copy your $(FULLNAME) ROMs here.
Supported extensions are: $(EXTENSIONS)

This system can be emulated using $(CORECOUNT) emulators.
These emulators are listed below, in order of compatibility/ease of use:
$(FOREACHCORE)$(EMULATOR)-$(CORE) supports files of $(COREEXTENTIONS) and is $(CORENETPLAY::NOT) Netplay compatible.

Device requirements:
- A controller is $(PAD:unusable:optional:recommended:required)
- A keyboard is $(KEYBOARD:unusable:optional:recommended:required)
- A mouse is $(MOUSE:unusable:optional:recommended:required)

$(TAGS:ZIP.ONEROM)

$(TAGS:DOC.TEXT)
$(DOCLINK)