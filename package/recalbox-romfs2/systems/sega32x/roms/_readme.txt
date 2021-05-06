=======================================================================
# RECALBOX: System $(FULLNAME) ($(TYPE:Arcade system:Home console:Handheld console:Computer:Virtual system))
=======================================================================

Copy your $(FULLNAME)'s roms here.
Supported extensions are: $(EXTENSIONS)

This systems can be emulated using $(CORECOUNT) emulators.
All emulators are listed below, in order of compatibility/ease to use:
$(FOREACHCORE)$(EMULATOR)-$(CORE) supports files of $(COREEXTENTIONS) and is $(CORENETPLAY::NOT) compatible netplay.

Device requirements:
- Pad is $(PAD:useless:optional:recommended:mandatory)
- Keyboard is $(KEYBOARD:useless:optional:recommended:mandatory)
- Mouse is $(MOUSE:useless:optional:recommended:mandatory)

$(TAGS:ZIP.ONEROM)

$(TAGS:DOC.TEXT)
$(DOCLINK)
