#!/bin/bash

totalAssertions=0
totalTests=0
MOCK_DIRECTORY=/tmp/bunitmocks/bin
MOCK_OUTPUT_FILE=/tmp/bunitmocks/output

# Use this function on test start.
## Usage:
### init nameOfTheTest [initMethod]
## Examples:
### init whenCallingThisReturnThat
### init whenCallingThisReturnThat initMethod
function init {
  totalTests=$((totalTests+1))
  currenttest=$1
  rm -rf "${MOCK_DIRECTORY}" "${MOCK_OUTPUT_FILE}"
  mkdir -p "${MOCK_DIRECTORY}"
  [ "$2" != "" ] && eval "$2"
}

# Asserts
function assertThat {
    totalAssertions=$((totalAssertions+1))
    case $2 in
    contains)
      if test "$(echo -e "$3")" != "$(cat $1)"; then
        echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        echo -e "Expected string: \n$3\ndo not equals to $1 content: \n$(cat $1)"
        exit 1
      fi
      ;;
    containsNumberOfLine)
      if test $3 -ne $(cat $1 | wc -l); then
        echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        echo -e "Expected number of lines: \n$3\ndo not equals to $1 number of lines: \n$(cat $1 | wc -l)"
        exit 1
      fi
      ;;
    exitedWith)
      if [[ -z "${VERBOSE}" ]];then
        eval "$1" &>/dev/null
      else
        eval "$1"
      fi
      returned=$?
      if test $returned -ne $3; then
        echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        echo -e "Expected $1 to return: \n$3\nbut returned:\n$returned"
        exit 1
      fi
      ;;
    hasBeenCalledWith)
      if ! cat $MOCK_OUTPUT_FILE | grep -q "$1 $3"; then
        echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        echo -e "Expected $1 to have been called with: \n$1 $3\nBut has been called:\n$(cat $MOCK_OUTPUT_FILE | grep $1)"
        exit 1
      fi
      ;;
    hasBeenCalled)
      if ! cat $MOCK_OUTPUT_FILE | grep -q "$1"; then
        echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        echo -e "Expected $1 to have been called"
        exit 1
      fi
      ;;
    *)
      echo "Assertion unknown" && echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
      ;;
    esac
}

# Mocks
function bunitPath {
    echo "${MOCK_DIRECTORY}"
}

function when {
  if [ ! -f "${MOCK_DIRECTORY}/${1}" ];then
    echo "#!/bin/bash" > "${MOCK_DIRECTORY}/${1}"
    chmod +x "${MOCK_DIRECTORY}/${1}"
  fi
  case $2 in
  isCalled)
    case $3 in
    thenEcho)
      echo "echo -e $4 && echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE}" >> "${MOCK_DIRECTORY}/${1}"
    ;;
    thenExit)
      echo "echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit $4" >> "${MOCK_DIRECTORY}/${1}"
    ;;
    with)
      case $5 in
      thenEcho)
        echo "[[ \"\$@\" == \"$4\" ]] && echo -e \"$6\" && echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit 0" >> "${MOCK_DIRECTORY}/${1}"
      ;;
      thenExit)
        echo "[[ \"\$@\" == \"$4\" ]] && echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit $6" >> "${MOCK_DIRECTORY}/${1}"
      ;;
      esac
    ;;
    *)
      echo "When unknown" && echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
    ;;
    esac
  ;;
  *)
    echo "When unknown" && echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
  ;;
  esac
}

# END
function end {
  echo -e "\e[32m✔\e[39m All $totalTests tests passed (with $totalAssertions assertions)"
  echo -e "Cleaning test files"
  rm -rf "${MOCK_DIRECTORY}" "${MOCK_OUTPUT_FILE}"
}