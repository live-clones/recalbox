#
# Check and build an existing chain of override file
#
from typing import List


def buildOverrideChain(rom, configfile) -> List[str]:
    import os
    romFolder = os.path.dirname(rom)
    items = romFolder.split('/')
    result: List[str] = []
    pathToCheck = "/" if romFolder.startswith("/") else ""

    # Build folder chain
    for item in items:
        pathToCheck = os.path.join(pathToCheck, item)
        fileToCheck = os.path.join(pathToCheck, configfile)
        if os.path.exists(fileToCheck):
            result.append(fileToCheck)

    # Add final file
    fileToCheck = "{}{}".format(rom, configfile)
    if os.path.exists(fileToCheck):
        result.append(fileToCheck)

    return result
