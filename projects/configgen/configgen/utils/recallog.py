#!/usr/bin/python
import inspect
import os


def recallog(message: str, hasEscapeChars=False, log_type=""):
    finalText = message
    log_type_arg = "-t '{}'".format(log_type) if log_type != "" else ""
    escape_char_args = "-e" if hasEscapeChars else ""

    os.system('/usr/bin/recallog -s CONFIGGEN {} {} "{}"'.format(log_type_arg, escape_char_args, finalText))
