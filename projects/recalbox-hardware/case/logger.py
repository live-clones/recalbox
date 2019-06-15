#!/usr/bin/python
import inspect

def hardlog(message):

    frame,filename,lineNumber,functionName,lines,index = inspect.stack()[1]
    idx = filename.index("case")
    filename = filename[idx:]
    finalText = "{}({}:{}): {}".format(filename, functionName, lineNumber, message)

    print(finalText)
    try:
        with open("/boot/hardware.log", "a+") as sf:
            sf.write(finalText + "\r\n")
    except Exception:
        try:
            os = __import__("os")
            os.system("mount -o remount,rw /boot")
            with open("/boot/hardware.log", "a+") as sf:
                sf.write(finalText + "\r\n")
        except Exception:
            pass

