#!/bin/bash
if [[ $# -ne 1 ]]; then
    echo "mismatch args"
    exit 2
fi

if [[ ! -d $1 ]]; then
    echo "Not directory"
    exit 1
fi

files=$(find ${1} -type f -name "*.tgz")
if [[ ! -e "${1}/hash.db" ]]; then
    touch ${1}/hash.db
fi
if [[ ! -d  "${1}/extracted" ]]; then
    mkdir ${1}/extracted
fi
tmpHash=$(mktemp)

HashDB="${1}/hash.db"
while read -r line; do
        name=$(basename ${line} | cut -d '-' -f1 | sed -E s:"_[^_]*$"::g)
        if [[ ${name} =~ _ ]]; then
            echo "CONTINUE:: ${name}"
            continue
        fi
        timestamp=$(echo "${line}" | cut -d '-' -f2 | sed s:".tgz"::g)
        hash=$(sha256sum ${line} | cut -d ' ' -f1)
        echo "Name: ${name}   Timestamp: ${timestamp}"
        echo "File : ${line}"
        echo "Hash: ${hash}"
        if grep -q -E "${hash}" ${HashDB}; then
            continue
        fi

        if tar -tzf ${line} | grep -q -E "meow.txt"; then
            path=$(tar -tzf ${line} | grep -E "meow.txt")
            tar -xOf ${line} ${path} > ${1}/extracted/${name}_${timestamp}.txt
        fi

        echo "${hash}" >> ${tmpHash}
done< <(echo "${files}")

cat ${tmpHash} | uniq > ${HashDB}
rm ${tmpHash}
