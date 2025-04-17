#!/bin/bash
if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -e ${1} ]]; then
    echo "first args doesnt exist"
    exit 2
fi

if [[ ! -d ${2} ]]; then
    echo "second args shouble be dict"
    exit 3
fi

if [[ $(find ${2} -type f | wc -l) -ne 0 ]]; then
    echo "Dir should be empty"
    exit 4
fi
touch ${2}/dict.txt
counter=1
while read -r name; do
    echo "${name} ${counter}" >> ${2}/dict.txt
    touch ${2}/${counter}.txt
    cat ${1} | grep -E "${name}" >> ${2}/${counter}.txt
    counter=$(echo "${counter} + 1" | bc)

done< <(cat ${1} | cut -d ':' -f1 | sed -E s:" \(.*\)$"::g | sort | uniq)
