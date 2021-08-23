#!/bin/bash

btlist="/tmp/btlist"
/recalbox/scripts/bluetooth/test-device list > $btlist

fast_chr() {
    local __octal
    local __char
    printf -v __octal '%03o' $1
    printf -v __char \\$__octal
    REPLY=$__char
}

function unichr {
    local c=$1    # Ordinal of char
    local l=0    # Byte ctr
    local o=63    # Ceiling
    local p=128    # Accum. bits
    local s=''    # Output string

    (( c < 0x80 )) && { fast_chr "$c"; echo -n "$REPLY"; return; }

    while (( c > o )); do
        fast_chr $(( t = 0x80 | c & 0x3f ))
        s="$REPLY$s"
        (( c >>= 6, l++, p += o+1, o>>=1 ))
    done

    fast_chr $(( t = p | c ))
    echo -n "$REPLY$s"
}

while IFS= read -r line
do
  prefix=`unichr 9654`' '
  #echo "$line"
  mac=`echo $line | cut -d ' ' -f1`
  #echo $mac
  if [ `/recalbox/scripts/bluetooth/test-device trusted $mac` -eq 1 ];then
    prefix=`unichr 9655`' '
  fi
  echo -e $prefix$line
done < "$btlist"
