import os
import shutil
import uuid


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
        systemIni = os.path.join(target, self.SYSTEMINI_FILE)

        # Create folders
        os.makedirs(target, exist_ok=True)

        # Copy files
        exist: bool = os.path.exists(systemIni)
        shutil.copytree(template, target, dirs_exist_ok=True)

        # Process system.ini
        self.__processSystemIni(target, systemIni, exist)

        print("[ROMFS 2] System {} created into {}".format(self.__name, self.__systemRoot))

    def __processSystemIni(self, target: str, systemIni: str, exist: bool):
        # read system.ini
        with open(systemIni, 'r') as f:
            content = f.read()

        # Set system short name
        content = content.replace("$(SYSTEM)", self.__name)
        content = content.replace("$(UUID)", str(uuid.uuid4()))

        # Write back
        if not exist or self.__force:
            with open(systemIni, 'w') as f:
                f.write(content)
