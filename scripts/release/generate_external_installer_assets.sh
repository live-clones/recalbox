#!/bin/bash

## FUNCTIONS ##

function exitWithUsage {
  echo "Usage: $0 --images-dir <images_dir> --destination-dir <destination_dir>"
  echo
  echo "This script generates assets for external OS installers (imagers)"
  echo "such as NOOBS, PINN or Raspberry Pi Imager."
  echo
  echo "  --images-dir <images_dir>            path to Recalbox images"
  echo "  --destination-dir <destination_dir>  path where assets will be generated"
  echo
  echo "The <images_dir> expects the following file hierarchy:"
  echo
  echo "    <images_dir>/"
  echo "    ├─ rpi1/"
  echo "    │  ├─ recalbox-rpi1.img.xz"
  echo "    │  └─ boot.tar.xz"
  echo "    ├─ rpi2/"
  echo "    │  ├─ recalbox-rpi2.img.xz"
  echo "    │  └─ boot.tar.xz"
  echo "    ├─ rpi3/"
  echo "    │  ├─ recalbox-rpi3.img.xz"
  echo "    │  └─ boot.tar.xz"
  echo "    └─ rpi4/"
  echo "       ├─ recalbox-rpi4.img.xz"
  echo "       └─ boot.tar.xz"
  echo
  exit 64
}

function generateNoobsAssets {
  local templateDir="$(dirname $(readlink -f $0))/templates/noobs"
  local destinationDir="${params[destinationDir]}/noobs"
  declare -A metadata

  echo ">>> Generating assets for NOOBS (in ${destinationDir})"

  # Fetch generic metadata

  metadata[version]=${CI_COMMIT_REF_NAME}
  metadata[releaseDate]=$(date +%Y-%m-%d)

  # Fetch tarball metadata

  for arch in rpi1 rpi2 rpi3 rpi4; do
    local tarball="${params[imagesDir]}/${arch}/recalbox-${arch}.tar.xz"
    local uncompressedTarballSizeInBytes=$(xz --robot --list "${tarball}" | tail -1 | cut -f 5)
    metadata["${arch}UncompressedTarballSize"]=$((${uncompressedTarballSizeInBytes} / 1024 / 1024))
    # Url is rewritten to https://upgrade.recalbox.com/latest/${arch}/recalbox-${arch}.tar.xz in proxies
    metadata["${arch}TarballUrl"]="https://upgrade.recalbox.com/latest/noobs/${arch}/recalbox-${arch}.tar.xz"
  done

  # Create assets in destination directory

  mkdir -p "${destinationDir}"
  cp "${templateDir}/recalbox.png" "${destinationDir}/recalbox.png"
  cp "${templateDir}/marketing.tar" "${destinationDir}/marketing.tar"
  cp "${templateDir}/marketing-kubii.tar" "${destinationDir}/marketing-kubii.tar"
  cp "${templateDir}/os_list_kubii.json" "${destinationDir}/os_list_kubii.json"

  cat "${templateDir}/os.json" \
  | sed -e "s|{{version}}|${metadata[version]}|" \
        -e "s|{{releaseDate}}|${metadata[releaseDate]}|" \
  > "${destinationDir}/os.json"

  for arch in rpi1 rpi2 rpi3 rpi4; do
    mkdir -p "${destinationDir}/${arch}"
    cat "${templateDir}/partitions.json" \
    | sed -e "s|{{uncompressedTarballSize}}|${metadata["${arch}UncompressedTarballSize"]}|" \
          -e "s|{{tarballUrl}}|${metadata["${arch}TarballUrl"]}|" \
    > "${destinationDir}/${arch}/partitions.json"
  done
}

function generateRaspberryPiImagerAssets {
  local templateDir="$(dirname $(readlink -f $0))/templates/rpi_imager"
  local destinationDir="${params[destinationDir]}/rpi-imager"
  declare -A metadata

  echo ">>> Generating assets for Raspberry Pi Imager (in ${destinationDir})"

  # Gather required information from images directory

  metadata[version]=${CI_COMMIT_REF_NAME}
  metadata[releaseDate]=$(date +%Y-%m-%d)

  for arch in rpi1 rpi2 rpi3 rpi4; do
    local imageFile="${params[imagesDir]}/${arch}/recalbox-${arch}.img.xz"
    # Fetch info regarding image downloads (XZ-compressed Recalbox image)
    metadata["${arch}ImageDownloadSize"]=$(stat -c '%s' "${imageFile}")
    metadata["${arch}ImageDownloadSha256"]=$(sha256sum "${imageFile}" | cut -d' ' -f1)
    # Fetch info regarding extracted images (raw Recalbox image, after XZ decompression)
    metadata["${arch}ExtractSize"]=$(xz --robot --list "${imageFile}" | tail -1 | cut -f 5)
    metadata["${arch}ExtractSha256"]=$(xz --decompress --keep --to-stdout "${imageFile}" | sha256sum - | cut -d' ' -f1)
  done

  # Create assets in destination directory

  mkdir -p ${destinationDir}

  cat "${templateDir}/os_list_imagingutility_recalbox.json" \
  | sed -e "s|{{version}}|${metadata[version]}|" \
        -e "s|{{releaseDate}}|${metadata[releaseDate]}|" \
        -e "s|{{rpi1ExtractSize}}|${metadata[rpi1ExtractSize]}|" \
        -e "s|{{rpi2ExtractSize}}|${metadata[rpi2ExtractSize]}|" \
        -e "s|{{rpi3ExtractSize}}|${metadata[rpi3ExtractSize]}|" \
        -e "s|{{rpi4ExtractSize}}|${metadata[rpi4ExtractSize]}|" \
        -e "s|{{rpi1ExtractSha256}}|${metadata[rpi1ExtractSha256]}|" \
        -e "s|{{rpi2ExtractSha256}}|${metadata[rpi2ExtractSha256]}|" \
        -e "s|{{rpi3ExtractSha256}}|${metadata[rpi3ExtractSha256]}|" \
        -e "s|{{rpi4ExtractSha256}}|${metadata[rpi4ExtractSha256]}|" \
        -e "s|{{rpi1ImageDownloadSize}}|${metadata[rpi1ImageDownloadSize]}|" \
        -e "s|{{rpi2ImageDownloadSize}}|${metadata[rpi2ImageDownloadSize]}|" \
        -e "s|{{rpi3ImageDownloadSize}}|${metadata[rpi3ImageDownloadSize]}|" \
        -e "s|{{rpi4ImageDownloadSize}}|${metadata[rpi4ImageDownloadSize]}|" \
        -e "s|{{rpi1ImageDownloadSha256}}|${metadata[rpi1ImageDownloadSha256]}|" \
        -e "s|{{rpi2ImageDownloadSha256}}|${metadata[rpi2ImageDownloadSha256]}|" \
        -e "s|{{rpi3ImageDownloadSha256}}|${metadata[rpi3ImageDownloadSha256]}|" \
        -e "s|{{rpi4ImageDownloadSha256}}|${metadata[rpi4ImageDownloadSha256]}|" \
  > "${destinationDir}/os_list_imagingutility_recalbox.json"
  cp "${templateDir}/recalbox.png" "${destinationDir}/recalbox.png"
}

## PARAMETERS PARSING ##

declare -A params

while [ -n "$1" ]; do
  case "$1" in
    --images-dir)
      shift
      [ -n "$1" ] && params[imagesDir]=$(readlink -f "$1") || exitWithUsage
      ;;
    --destination-dir)
      shift
      [ -n "$1" ] && params[destinationDir]=$(readlink -f "$1") || exitWithUsage
      ;;
    *)
      exitWithUsage
      ;;
  esac
  shift
done

if [[ ! -d ${params[imagesDir]} || ! -d ${params[destinationDir]} ]]; then
  exitWithUsage
fi

## MAIN ##

generateNoobsAssets
generateRaspberryPiImagerAssets
