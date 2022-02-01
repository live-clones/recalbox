#!/usr/bin/env python


class SubSystems:
    UNKNOWN = "unknown"
    A600 = "amiga600"
    A1200 = "amiga1200"
    CDTV = "amigacdtv"
    CD32 = "amigacd32"

    COMPUTERS = ( A600, A1200 )
    CONSOLES = ( CDTV, CD32 )
    HAVEMOUSE = ( A600, A1200, CDTV, CD32 )
