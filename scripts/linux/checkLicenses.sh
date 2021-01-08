#!/bin/bash

set -uo pipefail

OUTPUT="${1}"
SEPARATOR=""

LICENCES = 
echo '{"version": "2.1", "licenses": [{"id": "GPL3"},{"id": "MPL-2.0","name": "Mozilla Public License 2.0","url": "https://opensource.org/licenses/MPL-2.0"}],"dependencies" : [ ' > "${OUTPUT}"
for packageRaw in $(cat output/.config | grep -e "^BR2_PACKAGE" | sed 's/=.*//g;');do
	package="${packageRaw//BR2_PACKAGE_/}" 
	packageLower="${package,,}"
	packageDir="${packageLower//_/-}" 
	packageBRMk="./buildroot/package/${packageDir}/${packageDir}.mk"
	packageCustomMk="./package/${packageDir}/${packageDir}.mk"
	packageCustomMkUnderscore="./package/${packageDir}/${packageLower}.mk"

    file=""
    source=""
	if [ -f "${packageCustomMk}" ];then	
		file="${packageCustomMk}"
    	source="RECALBOX"
	elif [ -f "${packageCustomMkUnderscore}" ];then	
		file="${packageCustomMkUnderscore}"
    	source="RECALBOX"
	elif [ -f "${packageBRMk}" ];then	
    	file="${packageBRMk}"
    	source="BUILDROOT"
	else
		echo "SKIP: no make file found for ${package}"
		continue
	fi

	licenseLine=$(grep -e "^${package}_LICENSE =" "${file}")
	if [ $? != 0 ]; then
		echo "WARN: no license in ${file} from ${source}"
		continue
	fi
	versionLine=$(grep -e "^${package}_VERSION =" "${file}")
	if [ $? != 0 ]; then
		echo "WARN: no version in ${file} from ${source}"
		continue
	fi
	license=$(echo "${licenseLine//\\/}" | awk '{print $3}')
	version=$(echo "$versionLine" | awk '{print $3}')
	echo "INFO: $package $license"
	echo "${SEPARATOR}{ \"name\": \"${packageDir}\", \"version\": \"${version//$'\n'/}\", \"licenses\": [\"${license//$'\n'/}\"]}">> "${OUTPUT}"
	SEPARATOR=","
done
echo ']} ' >> "${OUTPUT}"
