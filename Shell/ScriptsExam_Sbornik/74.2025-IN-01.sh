#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "args mismatch"
    exit 2
fi
#if [[ $(id -u) -ne 0]]; then # root check has id 0 in etc/passwd
  #  echo "You are not root"
 #   exit 3
#fi
# find -type f,d
objects=$(mktemp)
while read -r dir type mode; do
    if [[ ${type} == "R" ]]; then
        find "${dir}" -mindepth 1 \( -type f -o -type d \) -perm "${mode}" -print0 >> ${objects}
    elif [[ ${type} == "A" ]]; then
        find "${dir}" -mindepth 1 \( -type f -o -type d \) -perm "/${mode}" -print0 >> ${objects}
    elif [[ ${type} == "T" ]]; then
       find "${dir}" -mindepth 1 \( -type f -o -type d \) -perm "-${mode}" -print0 >> ${objects}
    fi

done< <(cat "${1}" | tr -s ' ')

while IFS= read -r -d '' file; do
    if [[ -d "${file}" ]]; then
        chmod 755 "${file}"
    
    else
        chmod 664 ${file}
    fi
    echo "File: ${file}"
done< <(cat "${objects}")

rm ${objects}
