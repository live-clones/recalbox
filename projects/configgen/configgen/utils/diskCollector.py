from typing import List


class DiskCollector:

    def __init__(self, rom: str, maxDisk: int, verbose: bool):
        self.rom: str = rom
        self.maxDisk: int = maxDisk
        self.disks: List[str] = []
        self.verbose: bool = verbose
        self.scan()

    @property
    def Disks(self):
        return self.disks

    @property
    def Count(self):
        return len(self.disks)

    def scan(self):
        # Scan for disk number
        isDisk, _, masterPrefix, masterSuffix = self.scanDisk(self.rom)
        # No disk, return a single rom
        if not isDisk:
            self.disks.append(self.rom)
            return

        # Get disks with the same prefix
        diskList = self.getAllDisks(masterPrefix)
        # Find best matching disks
        currentDisk = 1
        suffixLen = len(masterSuffix)
        while currentDisk <= self.maxDisk:
            found = False
            # Try to match prefix and suffix
            for disk in diskList:
                isStillADisk, diskNumber, _, suffix = self.scanDisk(disk)
                if isStillADisk and diskNumber == currentDisk and suffix == masterSuffix:
                    self.disks.append(disk)
                    found = True
                    break
            # Try to match prefix and suffix len
            if not found:
                for disk in diskList:
                    isStillADisk, diskNumber, _, suffix = self.scanDisk(disk)
                    if isStillADisk and diskNumber == currentDisk and len(suffix) == suffixLen:
                        self.disks.append(disk)
                        found = True
                        break
                # Not found? Get the first disk available
                if not found:
                    for disk in diskList:
                        isStillADisk, diskNumber, _, _ = self.scanDisk(disk)
                        if isStillADisk and diskNumber == currentDisk:
                            self.disks.append(disk)
                            break
            currentDisk += 1

        if self.verbose:
            print("Original disk: {}".format(self.rom))
            print("Found {} disks".format(len(self.disks)))
            diskNumber = 1
            for disk in self.disks:
                print("  Disk #{} : {}".format(diskNumber, disk))
                diskNumber += 1

    @staticmethod
    def getAllDisks(suffix: str):
        diskPattern = suffix.replace("[", "|").replace("]", "[]]").replace("|", "[[]") + '*'
        import glob
        return glob.glob(diskPattern)

    @staticmethod
    def scanDisk(rom: str) -> (bool, int, str, str):
        try:
            romlower = rom.lower().replace('[', '(').replace(']', ')')
            start = romlower.find('(disk ')
            if start < 0:
                start = romlower.find('(disc ')
            if start > 0:
                stop = romlower.find(')', start)
                if stop > 0:
                    subParts = romlower[start:].split(' ')
                    if 'a' <= subParts[1] <= 'z':
                        return True, ord(subParts[1]) - ord('a') + 1, rom[0:start], rom[stop+1:]
                    return True, int(subParts[1]), rom[0:start], rom[stop+1:]
        except Exception as ex:
            print("Exception scanning {}: {}".format(rom, ex))

        return False, 1, '', ''
