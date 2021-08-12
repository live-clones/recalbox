#!/usr/bin/python
import inspect
import os


def recallog(message: str, has_escape_chars=False, log_type=""):
    final_text = message
    log_type_arg = "-t '{}'".format(log_type) if log_type != "" else ""
    escape_char_args = "-e" if has_escape_chars else ""

    os.system('/usr/bin/recallog -s CONFIGGEN {} {} "{}"'.format(log_type_arg, escape_char_args, final_text))
