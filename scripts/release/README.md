# NOOBS

<details>
  <summary>Recalbox entries for <code>os_list_v3.json</code></summary>

  > ℹ️ This can only be added to the [official `os_list_v3.json`](https://downloads.raspberrypi.org/os_list_v3.json) by Raspberry Foundation engineers

  ```json
  {
      "os_list": [
          {
              "os_name": "Recalbox - Pi0/1",
              "description": "The official retro-gaming OS! Turn your Raspberry Pi into an all-in-one and plug-n-play retro-gaming console, supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://download.recalbox.com/noobs/recalboxOS.png",
              "marketing_info": "https://download.recalbox.com/noobs/marketing.tar",
              "partition_setup": "https://download.recalbox.com/noobs/partition_setup.sh",
              "partitions_info": "https://download.recalbox.com/noobs/rpi1/partitions.json",
              "os_info": "https://download.recalbox.com/noobs/os.json",
              "supported_models": [
                  "Pi Model",
                  "Pi Compute Module Rev",
                  "Pi Zero"
              ],
              "tarballs": [
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi1/boot.tar.xz",
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi1/root.tar.xz"
              ]
          },
          {
              "os_name": "Recalbox - Pi2",
              "description": "The official retro-gaming OS! Turn your Raspberry Pi into an all-in-one and plug-n-play retro-gaming console, supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://download.recalbox.com/noobs/recalboxOS.png",
              "marketing_info": "https://download.recalbox.com/noobs/marketing.tar",
              "partition_setup": "https://download.recalbox.com/noobs/partition_setup.sh",
              "partitions_info": "https://download.recalbox.com/noobs/rpi2/partitions.json",
              "os_info": "https://download.recalbox.com/noobs/os.json",
              "supported_models": [
                  "Pi 2"
              ],
              "tarballs": [
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi2/boot.tar.xz",
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi2/root.tar.xz"
              ]
          },
          {
              "os_name": "Recalbox - Pi3",
              "description": "The official retro-gaming OS! Turn your Raspberry Pi into an all-in-one and plug-n-play retro-gaming console, supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://download.recalbox.com/noobs/recalboxOS.png",
              "marketing_info": "https://download.recalbox.com/noobs/marketing.tar",
              "partition_setup": "https://download.recalbox.com/noobs/partition_setup.sh",
              "partitions_info": "https://download.recalbox.com/noobs/rpi3/partitions.json",
              "os_info": "https://download.recalbox.com/noobs/os.json",
              "supported_models": [
                  "Pi 3",
                  "Pi Compute Module 3"
              ],
              "tarballs": [
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi3/boot.tar.xz",
                  "https://recalbox-releases.s3.nl-ams.scw.cloud/stable/v1/noobs/rpi3/root.tar.xz"
              ]
          }
      ]
  }
  ```
</details>

*TL;DR* NOOBS configuration is completely undocumented. Here is our attempt to document knowledge we gathered by digging into NOOBS source code, reading other OS entries in the [official `os_list_v3.json`](https://downloads.raspberrypi.org/os_list_v3.json) and experimenting.

## `os_list_v3.json`

* `os_name` ⇒ used as base name for the OS
* `nominal_size` ⇒ used by NOOBS to determine if the OS can be installed based on the available disk space
* `os_info` ⇒ URL of file downloaded as `os.json`
* `partitions_info` ⇒ URL of file downloaded as `partitions.json`
* `marketing_info` ⇒ URL of file downloaded as `marketing.tar` (images in it should be at least 400x200 pixels)
* `partition_setup` ⇒ URL of file downloaded as `partition_setup.sh`
* `icon` ⇒ URL of file downloaded as `icon.png` (must be 40x40 pixels)
* `flavours` ⇒ array of OS flavours, each item can contain (those fields will be merged with top-level fields and each flavour will be available as a separate OS in NOOBS):
  * `name`
  * `description`
  * `icon`
  * `feature_level` ⇒ ???
* `supported_models` ⇒ array of strings which at least one must be present in `/proc/device-tree/model` for the OS to be considered compatible with the hardware
  * `Pi Zero` matches all Pi Zero models (Zero, Zero W, Zero WH)
  * `Pi Model` matches all Pi1 models (A, B, A+, B+)
  * `Pi Model A` matches Pi1 A models (A, A+)
  * `Pi Model A+` matches Pi1 A+ model specifically
  * `Pi Model B` matches Pi1 B models (B, B+)
  * `Pi Model B+` matches Pi1 B+ model specifically
  * `Pi 2` matches all Pi2 models (there's only one at the moment, actually: B)
  * `Pi 3` matches all Pi3 models (A+, B, B+)
  * `Pi 3 Model A` matches Pi3 A models (there's only one at the moment, actually: A+)
  * `Pi 3 Model B` matches Pi3 B models (Pi3B, Pi3B+)
  * `Pi 3 Model B+` matches Pi3B+ model specifically
  * `Pi 4` matches all Pi4 models (there's only one at the moment, actually: B)
  * `Pi Compute Module` matches all Pi CM models (CM1, CM3, CM3 Lite, CM3+, CM3+ Lite)
  * `Pi Compute Module Rev` matches Pi CM1 model specifically
  * `Pi Compute Module 3` matches Pi CM3 models (CM3, CM3+, maybe Lite variants too)
  * `Pi Compute Module 3+` matches PiCM3+ model specifically (maybe CM3+ Lite variant too)
  * not sure how to match CM Lite variants specifically: no one does that and I don't own one to see what `/proc/device-tree/model` contains

* <details>
    <summary>💀 Legacy fields</summary>

    * `group` ⇒ seen in existing entries of `os_list_v3.json` but there's no sign of it in NOOBS code
  </details>

## `partitions.json`

This file is downloaded from URL defined by the `partitions_info` field in `os_list_v3.json` and is used to configure how `parted` and `mkfs` will partition the disk.

Fields read from `partitioninfo.cpp`:
* `filesystem_type` ⇒ `fat`/`swap`/`ntfs` (default is for "Linux native", _a.k.a_ ext3/4)
* `mkfs_options` ⇒ additional options passed to `mkfs`
* `label` ⇒ partition label, must match a tarball filename (from the `tarballs` field in `os_list_v3.json`)
* `tarball`
* `want_maximised` ⇒ take all available space
* `empty_fs` ⇒ created empty
* `offset_in_sectors`
* `partition_size_nominal` ⇒ partition size in MB
* `requires_partition_number`
* `uncompressed_tarball_size` ⇒ uncompressed tarball size in MB (used to display accurante download and installation progress)
* `active`

## `os.json`

This file is downloaded from URL defined by the `os_info` field in `os_list_v3.json` and is used to display some additional information in NOOBS graphical interface.

Fields read from `osinfo.cpp`:
* `name` ⇒ name of the OS
* `version` ⇒ version number or version string
* `description` ⇒ description of the OS, appended to the name as a new line
* `release_date` ⇒ release date in `YYYY-MM-DD` format
* `bootable`
* `riscos_offset`
* <details>
    <summary>💀 Legacy fields</summary>

    * `supported_revisions` ⇒ used for hardware compatibility detection ([replaced in 2016 by `supported_models`](https://github.com/raspberrypi/noobs/commit/11ba0b51db6fa48c85b5c8cd13ca757f64a6bb96))
    * `supported_hex_revisions` ⇒ used for hardware compatibility detection ([replaced in 2016 by `supported_models`](https://github.com/raspberrypi/noobs/commit/11ba0b51db6fa48c85b5c8cd13ca757f64a6bb96))
    * `kernel` ⇒ seen in existing entries of `os_list_v3.json` but there's no sign of it in NOOBS code
    * `url` ⇒ seen in existing entries of `os_list_v3.json` but there's no sign of it in NOOBS code
  </details>

# Raspberry Pi Imager

<details>
  <summary>Recalbox entry for <code>os_list_imagingutility.json</code></summary>

  > ℹ️ This can only be added to the [official `os_list_imagingutility.json`](https://downloads.raspberrypi.org/os_list_imagingutility.json) by Raspberry Foundation engineers

  ```json
  {
    "os_list": [
      {
        "name": "Recalbox",
        "description": "The official retro-gaming OS! Turn your Raspberry Pi into an all-in-one and plug-n-play retro-gaming console, supporting 100+ gaming systems!",
        "icon": "https://download.recalbox.com/raspi-imager/recalbox.svg",
        "subitems_url": "https://download.recalbox.com/raspi-imager/os_list_imagingutility_recalbox.json"
      }
    ]
  }
  ```
</details>

Raspberry Pi Imager is much more flexible than NOOBS' one, because it allows us to host (and thus dynamically update!) our own configuration file via `subitems_url`.

## `os_list_imagingutility.json`

Each entry in this file represents an OS. Here is the list of fields each entry can define:
  * `name` ⇒ name of the OS
  * `description` ⇒ longer description of the OS
  * `icon` ⇒ logo of the OS
  * `extract_size` ⇒ size of image in bytes, once uncompressed
  * `extract_sha256` ⇒ SHA256 hash of image, once uncompressed
  * `image_download_size` ⇒ size of downloaded image (compressed)
  * `image_download_sha256` ⇒ SHA256 hash of image (compressed)
  * `release_date` ⇒ release date in `YYY-MM-DD` format
  * `subitems_url` ⇒ URL to a file describing "flavours" of this OS: they can define exactly the same fields and will replace the entry declaring `subitems_url`

