#!/bin/bash

maxValue=0;
while read -r line; do
    if [[ ${line} =~ ^[1-9]+[0-9]*$ ]]; then
           if [[ ${line} -gt ${maxValue} ]]; then
                maxValue=${line}
           fi
    fi

done< <(cat ${1} | tr -d '-')
cat ${1} | grep -E "^-?${maxValue}" | sort | uniq
