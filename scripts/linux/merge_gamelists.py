import argparse
import glob
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom

class Merger:

    def __init__(self):
        self._filelist = []
        pass

    def prettify(self, elem):
        xmlstr = ET.tostring(elem, 'utf-8').replace('\n', '')
        while "> " in xmlstr:
          xmlstr = xmlstr.replace("> ", ">")
        while " <" in xmlstr:
          xmlstr = xmlstr.replace(" <", "<")
        reparsed = minidom.parseString(xmlstr)
        result = reparsed.toprettyxml(indent=' ', newl='\n', encoding="utf-8")
        return result

    def read(self, inputfolder):
        for f in glob.glob(os.path.join(inputfolder, "*_gamelist.xml")):
            print("Found {}".format(f))
            self._filelist.append(f)

    def write(self, outputfile):
        # Read
        utree = ET.Element("gameList")
        for f in self._filelist:
            print("Loading {}".format(f))
            for se in ET.parse(f).getroot():
                utree.append(se)
        # Write
        print("Writing {}".format(outputfile))
        with open(outputfile, "w") as f:
            f.write(self.prettify(utree).encode("utf-8"))
        # Cleanup
        for f in self._filelist:
            print("Removing {}".format(f))
            os.remove(f)


if __name__ == '__main__':
    # Parse command line
    parser = argparse.ArgumentParser(description='Gamelist merger')
    parser.add_argument("-i", "--input", help="Input foldet where to collect *_gamelist.xml", type=str, required=True)
    parser.add_argument("-o", "--output", help="Output gamelist file", type=str, required=True)
    #parser.add_argument("--port", help="This system is a port, not a regular system", action="store_true", required=False)

    args = parser.parse_args()

    merger = Merger()
    merger.read(args.input)
    merger.write(args.output)
