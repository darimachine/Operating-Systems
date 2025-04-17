#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "args mismatch"
    exit 2
fi

if [[ ! -f ${1} ]];then
    echo "file 1 doesnt exist"
    exit 3
fi

if [[ ! -d ${3} ]];then
    echo "file 3 must be directory"
    exit 5
fi

validFileNames=$(mktemp)
config=$(mktemp) # obedinenie na sudurjanie
while read -r file; do
    isValid=true;
    errorInLines=$(mktemp)
    counter=0
    while read -r line; do
        counter=$((${counter} + 1)) # counter za pazene vuv faila
        if [[ ${line} =~ ^# ]] || [[ ${line} =~ ^\{[^\}]*}\;$ ]] || [[ ${line} == "" ]]; then
            continue
        fi
        echo "Line ${counter}:${line}" >> "${errorInLines}"
        isValid=false
    done< <(cat "${file}")
    if [[ ${isValid} == "true" ]]; then
        echo "${file}" >> "${validFileNames}"
    else
        echo "Error in $(basename ${file})"
        while read -r error; do
            echo "${error}"
        done< ${errorInLines}
    fi

    rm ${errorInLines}
done< <(find ${3} -mindepth 1 -type f -name "*.cfg" 2>/dev/null)

while read -r valid; do
    cat "${valid}" >> "${config}"
    name=$(basename ${valid} | sed -E "s:(.*).cfg$:\1:g" )
    # echo "${name}"
    if ! grep -q -E "^${name}:" ${1}; then
        pw=$(pwgen 20 1)
        echo "${name}:${pw}" >> ${1}
        echo "${name}:${pw}"
    fi

done < ${validFileNames}

cat "${config}" > ${2}

rm ${config}
rm ${validFileNames}
