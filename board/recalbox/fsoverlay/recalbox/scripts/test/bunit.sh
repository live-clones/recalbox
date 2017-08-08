#!/bin/bash

totalAssertions=0
totalTests=0
MOCK_DIRECTORY=/tmp/bunitmocks/bin
MOCK_OUTPUT_FILE=/tmp/bunitmocks/output

# Use this function on /usr/bin/test start.
## Usage:
### init nameOfTheTest [initMethod]
## Examples:
### init whenCallingThisReturnThat
### init whenCallingThisReturnThat initMethod
function init {
  totalTests=$((totalTests+1))
  currenttest=$1
  /bin/rm -rf "${MOCK_DIRECTORY}" "${MOCK_OUTPUT_FILE}"
  /bin/mkdir -p "${MOCK_DIRECTORY}"
  [ "$2" != "" ] && eval "$2"
}

# Asserts
function assertThat {
    totalAssertions=$((totalAssertions+1))
    case $2 in
    # assertThat /tmp/theFile.txt contains "The /usr/bin/test String"
    contains)
      if /usr/bin/test "$(/bin/echo -e "$3")" != "$(/bin/cat $1)"; then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected string: \n$3\ndo not equals to $1 content: \n$(/bin/cat $1)"
        exit 1
      fi
      ;;
    # assertThat /tmp/theFile.txt containsNumberOfLine 10
    containsNumberOfLine)
      if /usr/bin/test $3 -ne $(/bin/cat $1 | wc -l); then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected number of lines: \n$3\ndo not equals to $1 number of lines: \n$(/bin/cat $1 | wc -l)"
        exit 1
      fi
      ;;
    # assertThat "/bin/echo ok" exitedWith 0
    exitedWith)
      if [[ -z "${VERBOSE}" ]];then
        eval "$1" &>/dev/null
      else
        eval "$1"
      fi
      returned=$?
      if /usr/bin/test $returned -ne $3; then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected $1 to return: \n$3\nbut returned:\n$returned"
        exit 1
      fi
      ;;
    # assertThat "/bin/echo ok" echoed "ok"
    # Fails if the first argument has not echoed the third argument
    echoed)
      result=$(eval "$1")
      returned=$?
      if [[ -n "${VERBOSE}" ]];then
        /bin/echo "$result"
      fi
      if [[ "$result" != "$3" ]];then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected $1 to /bin/echo: \n$3\nbut echoed:\n$result"
        exit 1
      fi
      #if /usr/bin/test $returned -ne $3; then
      #  /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
      #  /bin/echo -e "Expected $1 to return: \n$3\nbut returned:\n$returned"
      #  exit 1
      #fi
      ;;
    # assertThat curl hasBeenCalledWith "https://url-to-use.com/rpi1/recalbox.version
    # Fails if the first argument has not been called with the third argument
    hasBeenCalledWith)
      if ! /bin/cat $MOCK_OUTPUT_FILE | /bin/grep -q "$1 $3"; then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected $1 to have been called with: \n$1 $3\nBut has been called:\n$(/bin/cat $MOCK_OUTPUT_FILE | /bin/grep $1)"
        exit 1
      fi
      ;;
    # assertThat curl hasBeenCalled
    # Fails if the first argument has not been called
    hasBeenCalled)
      if ! /bin/cat $MOCK_OUTPUT_FILE | /bin/grep -q "$1"; then
        /bin/echo -e "\e[31m❌\e[39m Test failed: $currenttest"
        /bin/echo -e "Expected $1 to have been called"
        exit 1
      fi
      ;;
    *)
      /bin/echo "Assertion unknown" && /bin/echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
      ;;
    esac
}

# Mocks
function bunitPath {
    /bin/echo "${MOCK_DIRECTORY}"
}

function when {
  if [ ! -f "${MOCK_DIRECTORY}/${1}" ];then
    /bin/echo "#!/bin/bash" > "${MOCK_DIRECTORY}/${1}"
    /bin/chmod +x "${MOCK_DIRECTORY}/${1}"
  fi
  case $2 in
  isCalled)
    case $3 in
    thenEcho)
      /bin/echo "/bin/echo -e $4 && /bin/echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE}" >> "${MOCK_DIRECTORY}/${1}"
    ;;
    thenExit)
      /bin/echo "/bin/echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit $4" >> "${MOCK_DIRECTORY}/${1}"
    ;;
    with)
      case $5 in
      thenEcho)
        /bin/echo "[[ \"\$@\" == \"$4\" ]] && /bin/echo -e \"$6\" && /bin/echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit 0" >> "${MOCK_DIRECTORY}/${1}"
      ;;
      thenExit)
        /bin/echo "[[ \"\$@\" == \"$4\" ]] && /bin/echo \"$1 \$@\" >> ${MOCK_OUTPUT_FILE} && exit $6" >> "${MOCK_DIRECTORY}/${1}"
      ;;
      esac
    ;;
    *)
      /bin/echo "When unknown" && /bin/echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
    ;;
    esac
  ;;
  *)
    /bin/echo "When unknown" && /bin/echo -e "\n\n\e[31m❌\e[39m Tests failed"; exit 1
  ;;
  esac
}

# END
function end {
  /bin/echo -e "\e[32m✔\e[39m All $totalTests tests passed (total of $totalAssertions assertions)"
  /bin/echo -e "Cleaning test files"
  /bin/rm -rf "${MOCK_DIRECTORY}" "${MOCK_OUTPUT_FILE}"
}