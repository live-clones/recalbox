import argparse
import glob
import os
import subprocess
import sys
from typing import Dict


class PatchGenerator:

    def __init__(self, args):
        self.__master = args.master
        self.__recalbox = args.recalbox
        self.__target = args.target

    def Generate(self):
        originalCommit = self.GetForkCommit()
        orderedCommits, namedCommits = self.GetBranchCommits(originalCommit)
        diffContents = self.GetDiffContents(orderedCommits)
        self.RemovePreviousPatches()
        self.WriteNewPatches(orderedCommits, namedCommits, diffContents)
        print("Completed")

    def RemovePreviousPatches(self):
        files = glob.glob(os.path.join(self.__target, "*.patch"))
        for file in files:
            os.remove(file)

    def WriteNewPatches(self, orderedCommits, namedCommits, diffContents): # type (List[str], Dict[str, str], Dict[str, str])
        i = 0
        for commit in orderedCommits[1:]:
            n = os.path.basename(self.__target)
            c = namedCommits[commit]
            filename = os.path.join(self.__target, f"{n}-{i:04d}-{c}.patch")
            with open(filename, "w") as f:
                f.write(diffContents[commit])
            print("Written {} bytes into {}".format(len(diffContents[commit]), filename))
            i += 1

    @staticmethod
    def GetDiffContents(orderedCommits): # type (List[str]) -> Dict[str, str]
        previous = orderedCommits[0]
        diffs : Dict[str, str] = {}
        for commit in orderedCommits[1:]:
            patch = subprocess.check_output(['git', 'diff', '--unified', previous, commit]).decode('utf-8')
            diffs[commit] = patch
            previous = commit
        return diffs

    def GetForkCommit(self): # type: () -> str
        # Get the commit the recalbox branch is forked from
        originalCommit = subprocess.check_output(['git', 'merge-base', self.__master, self.__recalbox]).decode('utf-8').replace('\n', '')
        if originalCommit.startswith("Fatal") or len(originalCommit) != 40:
            print("Cannot retrieve commit from {}, the branch {} is forked from".format(self.__master, self.__recalbox))
            exit(1)
        print("Original fork commit: {}".format(originalCommit))
        return originalCommit

    def GetBranchCommits(self, originalCommit): # type (str) -> List[str], Dict[str, str]
        # Get subsequent commits
        output = subprocess.check_output(['git', 'cherry', '-v', self.__master, self.__recalbox]).decode('utf-8')
        lines = output.splitlines()
        orderedCommits = [ originalCommit ]
        namedCommits = { originalCommit:'fork commit' }
        for line in lines:
            try:
                hashHexa = line[2:42]
                commit = line[43:]
                commit = commit.replace(' ', '-')
                orderedCommits.append(hashHexa)
                namedCommits[hashHexa] = commit
                print("Commit: {} {}".format(hashHexa, commit))
            except:
                print("Malformed commit: {}".format(line))
                exit(1)

        return orderedCommits, namedCommits

if __name__ == '__main__':
    # Parse command line
    parser = argparse.ArgumentParser(description='Emulator Packager helper')
    parser.add_argument("-t", "--target", help="Target directory in which to generate patch files", type=str, required=True)
    parser.add_argument("-m", "--master", help="Master branch name. default 'master'", default="master", type=str, required=False)
    parser.add_argument("-r", "--recalbox", help="Recalbox branch from whuch to generate patch. default 'recalbox''", default="recalbox", type=str, required=False)

    args = parser.parse_args()

    generator = PatchGenerator(args)
    generator.Generate()

    exit(0)