#!/usr/bin/env python
import pytest

from mock_open import MockOpen
from pprint import pprint

from Nvidia.NvidiaInstaller import GpuError, NvidiaInstaller

SUPPORTED_GPUS_JSON = '\
{\
  "chips": [\
    {\
      "devid": "0x1234",\
      "name": "Unsupported dummy video card",\
      "legacybranch": "42",\
      "features": [\
        "feature1", "feature2"\
      ]\
    },\
    {\
      "devid": "0x4242",\
      "name": "Supported dummy video card",\
      "features": [\
        "feature1", "feature2"\
      ]\
    }\
  ]\
}'

def test_get_supported_gpus_file():
    ni = NvidiaInstaller("123")

    path = ni.get_supported_gpus_file()

    assert path == "/usr/lib/extra/nvidia-123/supported-gpus.json"


def test_set_search_path():
    ni = NvidiaInstaller("123")
    ni.set_search_path("/path/to/somewhere")

    assert ni.search_path == "/path/to/somewhere"


def test_load_supported_gpus_file(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))

    ni = NvidiaInstaller("123")
    ni.load_supported_gpus_file()

    assert ni.gpus_data["chips"][0]["devid"] == "0x1234"


def test_is_gpu_supported_false(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))

    ni = NvidiaInstaller("123")
    ni.load_supported_gpus_file()
    supported = ni.is_gpu_supported("0x1234")

    assert supported == False
    assert ni.legacybranch == "42"


def test_is_gpu_supported_true(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))

    ni = NvidiaInstaller("123")
    ni.load_supported_gpus_file()
    supported = ni.is_gpu_supported("0x4242")

    assert supported == True
    assert ni.legacybranch == ""


def test_is_gpu_supported_unknown(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))

    ni = NvidiaInstaller("123")
    ni.load_supported_gpus_file()
    supported = ni.is_gpu_supported("0x0000")

    assert supported == False
    assert ni.legacybranch == ""


def test_install_supported(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))
    mocker.patch.object(NvidiaInstaller, "do_install")

    ni = NvidiaInstaller("123")
    try:
        ni.install("0x4242")
    except Exception as e:
        pytest.fail(f"Exception thrown: {e}")


def test_install_unsupported(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))
    mocker.patch.object(NvidiaInstaller, "do_install")

    ni = NvidiaInstaller("123")
    try:
        ni.install("0x1234")
        pytest.fail("should not go here")
    except GpuError as e:
        assert e.message == "GPU supported in 42 only"
    except Exception as e:
        pytest.fail(f"Wrong exception: {e}")


def test_install_unknown(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data=SUPPORTED_GPUS_JSON))
    mocker.patch.object(NvidiaInstaller, "do_install")

    ni = NvidiaInstaller("123")
    try:
        ni.install("0x0000")
        pytest.fail("should not go here")
    except GpuError as e:
        assert e.message == "Controller not supported. Is GPU?"
    except Exception as e:
        pytest.fail(f"Wrong exception: {e}")

def test_get_file_list():
    ni = NvidiaInstaller("123")
    fl = ni.get_file_list()

    assert type(fl) is dict
    assert type(fl["files"]) is list
    assert type(fl["libraries"]) is list
