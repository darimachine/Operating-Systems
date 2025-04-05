#!/bin/bash

# args checks 1
# failname Exist check
# add number to the every row

if [[ ${#} -ne 1 ]]; then
    echo "args must be 1"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "must be valid file"
    exit 2
fi
counter=1
result=$(mktemp)
while read -r line; do

    withoutYear=$(echo "${line}" | cut -d '-' -f2-)
    echo "${counter}.${withoutYear}" >> ${result}
    counter=$(echo "${counter}+1"| bc)
done< <(cat ${1})

cat "${result}" | sort -k2 -t '"'
rm "${result}"
