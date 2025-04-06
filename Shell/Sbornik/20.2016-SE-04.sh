#!/bin/bash

isNumber(){
    [[ $1 =~ ^[1-9]+[0-9]*$ ]]
}

if [[ ${#} != 2 ]]; then
    echo "You must have 2 positional args"
    exit 1
fi

if ! isNumber ${1} ||  ! isNumber ${2}; then
    echo "One of the arguments is not number"
    exit 2
fi
mkdir a
mkdir b
mkdir c

while read -r file; do
    fileRow=$(cat ${file} | wc -l)
    if [[ ${fileRow} -lt  ${1} ]]; then
        mv ${file} a/
    elif [[ ${fileRow} -gt "${1}" && ${fileRow} -lt "${2}" ]]; then
        mv ${file} b/
    else
        mv ${file} c/
    fi

done < <(find . -type f -name "*.txt")




