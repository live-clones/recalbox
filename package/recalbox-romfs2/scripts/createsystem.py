import os
import shutil


class CreateSystem:

    TEMPLATE_DIR: str = ".templates/system"
    SYSTEMINI_FILE: str = "system.ini"

    def __init__(self, systemRoot: str, name: str, force: bool):
        self.__systemRoot = systemRoot
        self.__name = name
        self.__force = force
        pass

    def execute(self):
        # Target folders
        target: str = os.path.join(self.__systemRoot, self.__name)
        template: str = os.path.join(self.__systemRoot, self.TEMPLATE_DIR)

        # Create folders
        os.makedirs(target, exist_ok=True)

        # Copy files
        shutil.copytree(template, target, dirs_exist_ok=True)

        # Process system.ini
        self.__processSystemIni(target)

        print("[ROMFS 2] System {} created into {}".format(self.__name, self.__systemRoot))

    def __processSystemIni(self, target: str):
        # read system.ini
        systemIni = os.path.join(target, self.SYSTEMINI_FILE)
        with open(systemIni, 'r') as f:
            content = f.read()

        # Set system short name
        content = content.replace("$(SYSTEM)", self.__name)

        # Write back
        if not os.path.exists(systemIni) or self.__force:
            with open(systemIni, 'w') as f:
                f.write(content)
