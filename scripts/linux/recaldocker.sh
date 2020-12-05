#!/bin/bash

# Quick'n'dirty checks
if [[ -z "$ARCH" ]] ; then
  inferred_arch=$(basename $(pwd) | sed -nr 's/^recalbox-(.+)/\1/p')
  if [[ -f configs/recalbox-${inferred_arch}_defconfig ]]; then
    ARCH=${inferred_arch}
  else
    echo "You must set the ARCH variable (we could not infer it from directory name)." >&2
    exit 1
  fi
fi

docker build -t "recalbox-dev" .

NPM_PREFIX_OUTPUT_PATH="`pwd`/output/build/.npm"
mkdir -p "$NPM_PREFIX_OUTPUT_PATH" "${PWD}/dl" "${PWD}/host"

docker run -ti --rm --security-opt seccomp=unconfined \
	-w="$(pwd)" \
	-v "$(pwd):$(pwd)" \
	-v "$NPM_PREFIX_OUTPUT_PATH:/.npm" \
	-v "${PWD}/dl:/share/dl" \
	-v "${PWD}/host:/share/host" \
	-e "ARCH=${ARCH}" \
	-e "PACKAGE=${PACKAGE}" \
	-e "RECALBOX_VERSION=${RECALBOX_VERSION:-development}" \
	-e "GITLAB_TOKEN_THEMES=${GITLAB_TOKEN_THEMES}" \
	-e "GITLAB_TOKEN_BEEBEM=${GITLAB_TOKEN_BEEBEM}" \
	-e "GITLAB_TOKEN_RB5000=${GITLAB_TOKEN_RB5000}" \
	-e "GITLAB_TOKEN_WEBMANAGER3=${GITLAB_TOKEN_WEBMANAGER3}" \
	--user="`id -u`:`id -g`" \
	"recalbox-dev" ${@}
