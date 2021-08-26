#!/usr/bin/env python
from typing import Dict, Optional, List


class Command:
    def __init__(self, videomode, array, env: Optional[Dict[str, str]]=None, delay=0.5, cwdPath="/", postExec=None):
        self.videomode = videomode
        self.array = array
        self.env = env if env is not None else {}
        self.delay = delay
        self.cwdPath = cwdPath
        self.postExec = postExec
        
    def __str__(self):
        result: List[str] = []

        for varName, varValue in self.env.items():
            result.append("%s=%s" % (varName, varValue))
            
        for value in self.array:
            result.append(value)

        return " ".join(result)
