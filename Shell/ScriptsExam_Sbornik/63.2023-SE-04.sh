#!/bin/bash

if [[ ${#} -ne 1 ]]; then 
    echo "Invalid args"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "must be directory"
    exit 2
fi
#sha256sum ednakvi
hashes=$(mktemp)

find ${1} -type f 2>/dev/null | xargs -I {} sha256sum {} | tr -s ' ' > ${hashes}
memory=0
groups=0
cat ${hashes}
while read -r hash; do
    #echo "${hash}"
    files=$(cat ${hashes} | grep -E "^${hash}" | cut -d ' ' -f2)
    #echo "${files}"
    if [[ $(echo "${files}" | wc -l) -gt 1 ]]; then
        original=$(echo "${files}" | head -n 1)
        #echo "REPEATED: ${files} "
        while read -r toDel; do
            echo "Deleting: ${toDel}"
            fileSize=$(stat -c "%s" ${toDel})
            echo "FileSize: ${fileSize}"
            memory=$(echo "${memory} + ${fileSize}" | bc)
            rm ${toDel} 
            ln -s ${original} ${toDel}
        done< <(echo "${files}" | tail -n +2)
        groups=$(( ${groups} +1 ))
    fi
    

done< <(cat ${hashes} | cut -d ' ' -f1 | sort | uniq)

echo "total Freed Memory ${memory}"
echo "Groups dedublicated ${groups}"

