#!/bin/bash

SHOW_FULL=false
if [ "$2" = "-f" ]; then
    SHOW_FULL=true
fi

mapfile -t ADDED   < <(git diff "$1" |grep -E '^(\+)[^+]' | sed 's/^.//')
mapfile -t REMOVED < <(git diff "$1" |grep -E '^(\-)[^-]' | sed 's/^.//')

is_removed() {
  local to_check="$1"
  for x in "${REMOVED[@]}"; do
      [ "$to_check" = "$x" ] && return 0
  done
  return 1
}

is_added() {
  local to_check="$1"
  for x in "${ADDED[@]}"; do
      [ "$to_check" = "$x" ] && return 0
  done
  return 1
}

total_added=()
total_removed=()
total_changed=()

for i in "${ADDED[@]}"; do
  if is_added "$i" && ! is_removed "$i" ; then
    total_added+=("$i")
  fi 
done
for i in "${REMOVED[@]}"; do
  if ! is_added "$i" && is_removed "$i"; then
    total_removed+=("$i")
  fi 
done
for i in "${ADDED[@]}"; do
  if is_added "$i" && is_removed "$i"; then
    total_changed+=("$i")
  fi 
done

if [ ${#total_added[*]} -gt 0 ]; then
  echo "+++ added:"
  printf '\e[32m%s\e[0m\n' "${total_added[@]}"
fi

if [ ${#total_removed[*]} -gt 0 ]; then
  echo "--- removed:"
  printf '\e[31m%s\e[0m\n' "${total_removed[@]}"
fi

if [ $SHOW_FULL = true ] && [ ${#total_changed[*]} -gt 0 ]; then
  echo "§§§ Postition changed:"
  printf '\e[33m%s\e[0m\n' "${total_changed[@]}"
fi


