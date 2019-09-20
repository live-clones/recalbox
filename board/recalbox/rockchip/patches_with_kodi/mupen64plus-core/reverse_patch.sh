#!/usr/bin/env sh
swap() {
    sed -e "s/^$1/PPP/" -e "s/^$2/$1/" -e "s/^PPP/$2/"
}
file_header() {
    head -2 $1 | tac | swap +++ ---
}
fix_chunk_header() {
    sed -e "s/@@ -\([0-9]\+,[0-9]\+\) +\([0-9]\+,[0-9]\+\) @@/@@ -\2 +\1 @@/" 
}
fix_lines() {
    swap + -
}
file="$1"
file_header $file
tail $file -n+3 | fix_chunk_header | fix_lines
