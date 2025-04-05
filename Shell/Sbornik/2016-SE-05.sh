#!/bin/bash

# 2 args check filename
# check if file exist
# compare sudurjashti negotovo ime na faila 
#which file has more lines diff or comm


if [[ ${#} -ne 2 ]]; then
    echo "args shoud be 2"
    exit 1
fi
if ! [[ -e ${1} && -e ${2} ]]; then
    echo "file doesnt exist"
    exit 2
fi
winner() {
    file=${1}
    fileContent=$(cat ${1} | tr -s ' ' | cut -d ' ' -f4-)
    echo "${fileContent}" | sort > "${1}.songs"
}

fileOneLines=$(grep -E "${1}" ${1} | wc -l)
fileTwoLines=$(grep -E "${2}" ${2} | wc -l)

if [[ ${fileOneLines} -gt ${fileTwoLines} ]]; then
    winner ${1}
    else
    winner ${2}
fi
