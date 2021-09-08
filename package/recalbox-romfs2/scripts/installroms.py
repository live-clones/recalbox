import glob
import os
import shutil
from typing import Dict, List, Union

from inisettings import IniSettings
from configin import ConfigIn
from systemholder import SystemHolder


class InstallRoms:

    __TAGS_FILE: str = ".templates/tags.ini"

    __README_FILES: Dict[str, str] = {
        "_readme.txt": "en",
        "_lisezmoi.txt": "fr",
        "_leiame.txt": "pt",
        "_leggime.txt": "it",
        "_liesmich.txt": "de",
    }

    __NULL_CORE = SystemHolder.Core("", 0, "", "", "", False, "", "")

    def __init__(self, systemRoot: str, target: str, root: str):
        self.__systemRoot = systemRoot
        self.__targetInit = target + "/share_init/roms"
        self.__targetUpgrade = target + "/share_upgrade/roms"
        self.__tags = IniSettings(os.path.join(systemRoot, self.__TAGS_FILE), True)
        self.__tags.loadFile(True)
        self.__config = ConfigIn(root)
        self.__arch = self.__config.Arch

    def execute(self):
        print("[ROMFS 2] Copying systems and metadata to {}".format(self.__targetInit))

        # Run though systems folders
        for folder in os.listdir(self.__systemRoot):
            subfolder = os.path.join(self.__systemRoot, folder)
            if os.path.isdir(subfolder) and not folder.startswith('.'):
                holder = SystemHolder(self.__arch, os.path.join(self.__systemRoot, folder, "system.ini"), self.__config)
                if holder.CoreCount < 1:
                    continue
                print("[ROMFS 2] Copying system {}".format(folder))
                self.__copyRomContent(subfolder, holder.RomFolder.replace("%ROOT%", self.__targetInit) if holder.IsPort else folder)
                print("[ROMFS 2] Creating readme files for system {}".format(folder))
                self.__installReadMe(folder, holder)

    def __copyRomContent(self, systemFolder: str, folder: str):
        # Copy roms
        roms: str = os.path.join(systemFolder, "init/roms")
        system: str = os.path.join(self.__targetInit, folder)
        shutil.rmtree(system, ignore_errors=True)
        shutil.copytree(roms, system, dirs_exist_ok=False)

    def __installReadMe(self, folder: str, holder: SystemHolder):
        targetSystemPath: str = os.path.join(self.__targetUpgrade, holder.RomFolder.replace("%ROOT%", self.__targetUpgrade) if holder.IsPort else folder)
        sourceSystemPath: str = os.path.join(self.__systemRoot, folder)
        templatePath: str = os.path.join(self.__systemRoot, ".templates/system/roms")
        for fileName, language in self.__README_FILES.items():
            self.__removePreviousFiles(sourceSystemPath, fileName, language)
            self.__installParsedFile(templatePath, sourceSystemPath, targetSystemPath, fileName, language, holder)

    @staticmethod
    def __find(name: str, path: str) -> Union[str, None]:
        for root, dir, files in os.walk(path):
            if name in files:
                return os.path.join(root, name)
        return None

    def __removePreviousFiles(self, source: str, fileName: str, language: str):
        # Clean any existing readme files if it exists in upgrade directory only
        source: str = source + "/upgrade"
        filePath: str = self.__find(fileName, source)
        if filePath != None and "/upgrade/" in filePath:
            if (os.path.exists(filePath)):
                os.remove(filePath)

    def __installParsedFile(self, templatePath: str, source: str, destinationPath: str, filename: str, language: str, holder: SystemHolder):
        # read file lines
        textPath = os.path.join(source, "upgrade/roms")
        filePath = self.__find(filename, textPath)
        if filePath is None:
            filePath = os.path.join(templatePath, filename)
            if not os.path.exists(filePath):
                raise FileNotFoundError("{} not found in {} !".format(textPath, filename))
        with open(filePath, 'r') as f:
            lines: List[str] = f.readlines()

        # Parse!
        newLines: List[str] = []
        for line in lines:
            newLines.extend(self.__parse(line, holder, language))

        # Write back
        filePath: str = self.__find(".readme.placeholder", source)
        if filePath is not None:
            # Some systems don't need to have any readme files, like the read-only ports
            filePath: str = os.path.join(os.path.dirname(filePath), filename)
            with open(filePath, 'w') as f:
                f.writelines(newLines)

            # Copy readme files
            roms: str = os.path.join(source, "upgrade/roms")
            system: str = os.path.join(self.__targetUpgrade, destinationPath)
            shutil.rmtree(system, ignore_errors=True)
            shutil.copytree(roms, system, dirs_exist_ok=False)

    def __parse(self, line: str, holder: SystemHolder, language: str) -> List[str]:
        # Core loop?
        if line.startswith("$(FOREACHCORE)"):
            # Sort cores
            cores: List[SystemHolder.Core] = []
            for emulator in holder.Cores:
                for core in holder.Cores[emulator]:
                    cores.append(core)
            cores.sort(key=lambda c: c.priority)
            # Prepare lines
            lines: List[str] = []
            line = line.replace("$(FOREACHCORE)", "")
            for core in cores:
                newline = line.replace("$(EMULATOR)", core.emulator)
                newline = newline.replace("$(CORE)", core.core)
                newline = newline.replace("$(COREEXTENTIONS)", ' '.join(core.extensions))
                newline = self.__parseTags(newline, language, holder)
                newline = self.__parseCommon(newline, holder, language)
                newline = self.__parseParameterized(newline, holder, core)
                lines.append(newline)
            return lines
        else:
            notEmpty: bool = line != '\n'
            line = self.__parseTags(line, language, holder)
            line = self.__parseCommon(line, holder, language)
            line = self.__parseParameterized(line, holder, self.__NULL_CORE)
            if notEmpty and line == '\n': return []
            return [line]

    @staticmethod
    def __parseParameterized(line: str, holder: SystemHolder, core: SystemHolder.Core) -> str:
        while True:
            start: int = line.find('$(')
            if start < 0 : return line
            stop: int = line.find(')', start)
            if stop < 0 : return line
            dotdot: int = line.count(':')
            if dotdot < 2: return line
            inner = line[start + 2: stop]
            chunks = inner.split(':')
            replacement = "<UNKNOWN_PARAMETERIZED_VARIABLE>"
            if chunks[0] == "CORENETPLAY" and len(chunks) == 3: replacement = chunks[1 if core.netplay else 2]
            elif chunks[0] == "PAD" and len(chunks) == 5: replacement = chunks[1 + holder.Properties.PadEnum]
            elif chunks[0] == "KEYBOARD" and len(chunks) == 5: replacement = chunks[1 + holder.Properties.KeyboardEnum]
            elif chunks[0] == "MOUSE" and len(chunks) == 5: replacement = chunks[1 + holder.Properties.MouseEnum]
            elif chunks[0] == "TYPE" and len(chunks) == 8: replacement = chunks[1 + holder.Properties.TypeEnum]
            line = line.replace(line[start: stop + 1], replacement)

    @staticmethod
    def __parseCommon(line: str, holder: SystemHolder, language: str) -> str:
        line = line.replace("$(NAME)", holder.Name)
        line = line.replace("$(FULLNAME)", holder.FullName)
        line = line.replace("$(EXTENSIONS)", holder.Extensions)
        line = line.replace("$(CORECOUNT)", str(holder.CoreCount))
        line = line.replace("$(DOCLINK)", holder.DocLinks(language))
        return line

    def __parseTags(self, line: str, language: str, holder: SystemHolder) -> str:
        # Special documentation tags
        docText: str = ""
        if len(holder.DocLinks(language)) != 0:
            docText = self.__tags.getOption("doc", "doc.text." + language, "").replace("\\n", "\n")
        line = line.replace("$(TAGS:DOC.TEXT)", docText)

        # Special zip tags
        zipText: str = ""
        if holder.Properties.Type != "arcade" and (".zip" in holder.Extensions or ".7z" in holder.Extensions):
            zipText = self.__tags.getOption("doc", "zip.onerom." + language, "").replace("\\n", "\n")
        line = line.replace("$(TAGS:ZIP.ONEROM)", zipText)

        # All other tags
        while True:
            start: int = line.find('$(TAGS:')
            if start < 0 : return line
            stop: int = line.find(')', start)
            if stop < 0 : return line
            tag = line[start + 7: stop].lower() + '.' + language
            line = line.replace(line[start : stop + 1], self.__tags.getOption("doc", tag, "").replace("\\n", "\n"))

