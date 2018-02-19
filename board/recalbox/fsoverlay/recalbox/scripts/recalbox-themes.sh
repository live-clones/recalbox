#!/bin/bash

upGradeToRecalboxNextTheme() {
    tmpFile=/tmp/es_setting.cfg.tmp
    # Set theme as recalbox-next + get default values from the share_init version
    # 1st rename ThemeSet value to recalbox-next
    # 2nd remove the last tag
    # 3rd add the required lines from the share_init version
    # close XML
    ( \
	sed 's+name="ThemeSet" value="recalbox"+name="ThemeSet" value="recalbox-next"+' /recalbox/share/system/.emulationstation/es_settings.cfg | \
        sed '/<\/config>/d' ; \
        grep -E 'name="ThemeMenu|ThemeSystemView|ThemeIconSet|ThemeGamelistView|ThemeColorSet"' /recalbox/share_init/system/.emulationstation/es_settings.cfg ; \
        echo "</config>"
    ) | xmllint --format - > $tmpFile

    # If all of this has succeeded, itmeans the resulting file is valid and we can upgrade the user file
    if [[ $? == 0 ]] ; then
        cp $tmpFile /recalbox/share/system/.emulationstation/es_settings.cfg
        return 0
    fi
    return 1
}

if grep -q 'name="ThemeSet" value="recalbox"' /recalbox/share/system/.emulationstation/es_settings.cfg ; then
    recallog "Upgrading theme to recalbox-next"
    upGradeToRecalboxNextTheme && recallog "recalbox-next Succeeded !" || recallog "recalbox-next failed !"
fi
