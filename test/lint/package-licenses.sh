#!/bin/bash

set -uo pipefail

exitCode=0
for packageRaw in $(ls package);do
	packageUpper="${packageRaw^^}"
	package="${packageUpper//-/_}" 
	packageDir="./package/${packageRaw}"

	file=""
	source=""
	if [ ! -f "${packageDir}"/*.mk ];then	
		echo "SKIP: no make file found for ${package}"
		continue
	fi

	license=""
	licenseLine=$(grep -e "^${package}_LICENSE =" "${packageDir}"/*.mk)
	if [[ $? != 0 ]]; then
		echo "WARN: no license in ${packageDir}/*.mk"
		exitCode=1
	else 
		license=$(echo "${licenseLine//\\/}" | awk '{print $3}')
	fi

	echo "INFO: name: $package license: $license"
done

exit ${exitCode}
