#!/bin/bash
if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[! -f ${1} ]]; then
    echo "file 1 doesnt exist"
    exit 2
fi
if [[ ! -f ${2} ]]; then
    echo "file 2 doesnt exist"
    exit 3
fi

while read -r line; do
    currentRows=$(cat ${1} | grep -E "[0-9]+,${line}")
    if [[ $(echo "${currentRows}" | wc -l) -gt 1 ]]; then
        echo "${currentRows}" | sort -n | head -n 1 >> ${2}
    else
        echo "${currentRows}" >> ${2}
    fi

done< <(cat ${1} | cut -d ',' -f2- | sort | uniq)
