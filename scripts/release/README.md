# NOOBS

<details>
  <summary>Recalbox entries for <code>os_list_v3.json</code></summary>

  > ℹ️ This can only be added to the [official `os_list_v3.json`](https://downloads.raspberrypi.org/os_list_v3.json) by Raspberry Foundation engineers

  ```json
  {
      "os_list": [
          {
              "os_name": "Recalbox - Pi0 / Pi1 / GPiCase",
              "description": "The retro-gaming OS! Supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://upgrade.recalbox.com/latest/noobs/recalbox.png",
              "marketing_info": "https://upgrade.recalbox.com/latest/noobs/marketing.tar",
              "partitions_info": "https://upgrade.recalbox.com/latest/noobs/rpi1/partitions.json",
              "os_info": "https://upgrade.recalbox.com/latest/noobs/os.json",
              "supported_models": [
                  "Pi Model",
                  "Pi Compute Module Rev",
                  "Pi Zero"
              ]
          },
          {
              "os_name": "Recalbox - Pi2",
              "description": "The retro-gaming OS! Supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://upgrade.recalbox.com/latest/noobs/recalbox.png",
              "marketing_info": "https://upgrade.recalbox.com/latest/noobs/marketing.tar",
              "partitions_info": "https://upgrade.recalbox.com/latest/noobs/rpi2/partitions.json",
              "os_info": "https://upgrade.recalbox.com/latest/noobs/os.json",
              "supported_models": [
                  "Pi 2"
              ]
          },
          {
              "os_name": "Recalbox - Pi3",
              "description": "The retro-gaming OS! Supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://upgrade.recalbox.com/latest/noobs/recalbox.png",
              "marketing_info": "https://upgrade.recalbox.com/latest/noobs/marketing.tar",
              "partitions_info": "https://upgrade.recalbox.com/latest/noobs/rpi3/partitions.json",
              "os_info": "https://upgrade.recalbox.com/latest/noobs/os.json",
              "supported_models": [
                  "Pi 3",
                  "Pi Compute Module 3"
              ]
          },
          {
              "os_name": "Recalbox - Pi4 / Pi400",
              "description": "The retro-gaming OS! Supporting 100+ gaming systems!",
              "nominal_size": 4096,
              "icon": "https://upgrade.recalbox.com/latest/noobs/recalbox.png",
              "marketing_info": "https://upgrade.recalbox.com/latest/noobs/marketing.tar",
              "partitions_info": "https://upgrade.recalbox.com/latest/noobs/rpi4/partitions.json",
              "os_info": "https://upgrade.recalbox.com/latest/noobs/os.json",
              "supported_models": [
                  "Pi 4"
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
* `tarballs` ⇒ array of tarball URLs to be uncompressed in created partitions (they are assigned to partitions defined in `partitions.json` in the same order : 1st tarball in `tarballs` is assigned to 1st partition in `partitions.json`, 2nd tarball in `tarballs` is assigned to 2nd partition in `partitions.json`, etc… and they override any `tarball` directive defined in `partitions.json`!)
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

This file is downloaded from URL defined by the `partitions_info` field in `os_list_v3.json` and is used to configure how to partition the disk then eventually create and populate filesystems into those partitions.

Fields (mostly read from `partitioninfo.cpp`):
* `filesystem_type` ⇒ filesystem type to create in the partition, can be one of:
  * `fat`/`ntfs`/`ext4`: filesystem will be created using some flavour or `mkfs` and content from the tarball will be un-tar'ed into it
  * `raw`: `mkfs` won't be used, filesystem will be copied (using `dd`) from the `tarball` (not actually a tarball in this case: just a XZ-compressed raw disk image)
  * `partclone`: `mkfs` won't be used, filesystem will be copied (using `partclone.restore`) from the `tarball` (not actually a tarball in this case: just a XZ-compressed [partclone](https://partclone.org) image)
  * `unformatted`: `mkfs` won't be used, filesystem won't be created in the partition
* `mkfs_options` ⇒ additional options passed to `mkfs`
* `label` ⇒ partition label
* `tarball` ⇒ URL of tarball to download (unless `filesystem_type` is `unformatted`), can be overriden if `tarballs` is present and contains enough entries (see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/mainwindow.cpp#L1567-L1580))
* `want_maximised` ⇒ take all available space (available space will be equally split between all partitions with `want_maximised` set to `true`, see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/multiimagewritethread.cpp#L136-L137))
* `empty_fs` ⇒ create partition and filesystem but don't populate it using a tarball
* `offset_in_sectors`
* `partition_size_nominal` ⇒ partition size in MB (or "minimum partition size", when `want_maximised` is `true`)
* `requires_partition_number` ⇒ claim a specific partition number (must be unique, must not be 1 nor 5, and the same OS cannot require both partition 2 and partition 4, see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/multiimagewritethread.cpp#L101-L121))
* `uncompressed_tarball_size` ⇒ uncompressed tarball size in MB (used to display accurate download and installation progress)
* `active`
* `partition_type` ⇒ partition type written in the partition table, actually represents the expected filesystem in the partition (see https://en.wikipedia.org/wiki/Partition_type) ; if unset, it can be infered from `filesystem_type` (if `filesystem_type` is `swap` or contains `fat` or `ntfs`, and defaults to Linux native `ext4` otherwise, see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/partitioninfo.cpp#L18-L28))

NOOBS actually creates:
* partition 1 (primary) -> FAT32 partition labeled RECOVERY (~900MB)
* partition 2 (extended), which contains:
  * partition 5 (logical) -> EXT4 partition labeled SETTINGS (32MB) for NOOBS data persistence
  * … here NOOBS inserts more logical partitions based on the content of `partitions.json` of each OS it installs (if many partitions have `want_maximised`, they will share the available space equally, as we can see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/multiimagewritethread.cpp#L136-L137))

> _Note: extended partition may be partition 4 if partition 2 is claimed by one of the installed OS using `requires_partition_number` (see [here in NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/multiimagewritethread.cpp#L357))

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

## `partition_setup.sh`

This script is started by NOOBS after creating all partitions, with partitions as parameters (both ARGV parameters and as environment variables).

This is a [comment from NOOBS source code](https://github.com/raspberrypi/noobs/blob/134417fba944a44c3a1b1de42c290d8bdfd0c6f5/recovery/multiimagewritethread.cpp#L576-L582) that documents it:

> Parameters to the partition-setup script are supplied both as
> command line parameters and set as environement variables.
>
> Boot partition is mounted, working directory is set to its mnt folder.
>
> `partition_setup.sh part1=/dev/mmcblk0p3 id1=LABEL=BOOT part2=/dev/mmcblk0p4 id2=UUID=550e8400-e29b-41d4-a716-446655440000`

# Raspberry Pi Imager

<details>
  <summary>Recalbox entry for <code>os_list_imagingutility.json</code></summary>

  > ℹ️ This can only be added to the [official `os_list_imagingutility.json`](https://downloads.raspberrypi.org/os_list_imagingutility.json) by Raspberry Foundation engineers

  ```json
  {
    "os_list": [
      {
        "name": "Recalbox",
        "description": "The retro-gaming OS! Supporting 100+ gaming systems!",
        "icon": "https://upgrade.recalbox.com/latest/rpi-imager/recalbox.png",
        "subitems_url": "https://upgrade.recalbox.com/latest/rpi-imager/os_list_imagingutility_recalbox.json"
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
  * `image_download_size` ⇒ size of downloaded image (compressed) in bytes
  * `image_download_sha256` ⇒ SHA256 hash of image (compressed)
  * `release_date` ⇒ release date in `YYY-MM-DD` format
  * `subitems_url` ⇒ URL to a file describing "flavours" of this OS: they can define exactly the same fields and will replace the entry declaring `subitems_url`

