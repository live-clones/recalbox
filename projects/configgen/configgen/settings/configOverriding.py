# Check and build an existing chain of o
#
#
def buildOverrideChain(rom, configfile):
    import os
    romFolder = os.path.dirname(rom)
    items = romFolder.split('/')
    result = []
    pathToCheck = "/" if romFolder.startswith("/") else ""

    # Build folder chain
    for item in items:
        pathToCheck = os.path.join(pathToCheck, item)
        fileToCheck = os.path.join(pathToCheck, configfile)
        #print(fileToCheck)
        if os.path.exists(fileToCheck):
            result.append(fileToCheck)

    # Add final file
    fileToCheck = "{}{}".format(rom, configfile)
    #print(fileToCheck)
    if os.path.exists(fileToCheck):
        result.append(fileToCheck)

    #print(result)
    return result
