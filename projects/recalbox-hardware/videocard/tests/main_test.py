#!/usr/bin/env python
import pytest
import main

from mock_open import MockOpen
from pprint import pprint

def test_parse_arguments():
    args = main.parse_arguments(
        ["--search-path", "/path/to/somewhere", "--version", "123"]
    )
    assert args.search_path == "/path/to/somewhere"
    assert args.version == "123"
