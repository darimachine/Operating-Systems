#!/bin/bash
sumDigits=$(mktemp)

while read -r num; do
   
    if [[ ${num} =~ ^-?[1-9]+[0-9]*$ ]]; then
        digitSum=$(echo "${num}" | tr -d '-'| grep -o . | awk '{sum+=$1} END {print sum}')
        #echo "${num} ${digitSum}"      
        echo "${num} ${digitSum}" >> ${sumDigits}
    fi

done< <(cat ${1}| sort | uniq)

cat ${sumDigits} | sort -k2nr -k1n | head -n 1
rm ${sumDigits}
