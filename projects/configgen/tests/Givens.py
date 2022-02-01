import builtins
from pathlib import Path
from typing import Dict
from unittest.mock import mock_open


def givenThoseFiles(mocker, files: Dict[str, str]):
    real = builtins.open
    def my_open(filename, mode):
        if filename in files.keys():
            content = files[filename]
            file_object = mock_open(read_data=content).return_value
            file_object.__iter__.return_value = content.splitlines(True)
            return file_object
        else:
            return real(filename, mode)

    mocker.patch('builtins.open', new=my_open)
