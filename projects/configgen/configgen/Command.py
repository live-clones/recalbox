#!/usr/bin/env python

class Command:
    def __init__(self, videomode, array, env=dict(), delay = 0.5, cwdPath = "/", postExec = None):
        self.videomode = videomode
        self.array = array
        self.env = env
        self.delay = delay
        self.cwdPath = cwdPath
        self.postExec = postExec
        
    def __str__(self):
        str = list()

        for varName, varValue in self.env.items():
            str.append("%s=%s" % (varName, varValue))
            
        for value in self.array:
            str.append(value)

        return " ".join(str)
