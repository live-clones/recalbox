#!/usr/bin/python
import inspect
import os

def recallog(message, hasEscapeChars=False):
    frame,filename,lineNumber,functionName,lines,index = inspect.stack()[1]
    finalText = "configgen/{}({}:{}): {}".format(os.path.basename(filename), functionName, lineNumber, message)

    if not hasEscapeChars:
        os.system('/usr/bin/recallog "' + finalText + '"')
    else:
        os.system('/usr/bin/recallog -e "' + finalText + '"')
