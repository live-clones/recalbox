#!/bin/bash -e

while [[ $# -gt 1 ]]
do
key="$1"
case ${key} in
    --from-changelog)
    CHANGELOG1="$2"
    shift
    ;;
    --to-changelog)
    CHANGELOG2="$2"
    shift
    ;;
    *)
    ;;
esac
shift
done

if [[ -z ${CHANGELOG1} || -z ${CHANGELOG2} ]]; then
    echo -e "Usage:\n$0 --from-changelog /path/to/changelog --to-changelog /path/to/changelog" && exit 1
fi

OLD_CHANGELOG="$(cat "${CHANGELOG1}" || exit 1)"
NEW_CHANGELOG="$(cat "${CHANGELOG2}" || exit 1)"

changes="$(diff --changed-group-format='%>' --unchanged-group-format='' <(echo "${OLD_CHANGELOG}") <(echo "${NEW_CHANGELOG}") || true)"

echo -ne "${changes}"

exit 0