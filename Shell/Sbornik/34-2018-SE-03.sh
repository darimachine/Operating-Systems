#!/bin/bash

if [[ ${#} != 2 ]]; then
    echo "argumentCount mismatch"
    exit 1
fi

if [[ ! -e ${1} ]]; then
    echo "first Argument doesnt exist"
    exit 2
fi

if [[ -e ${2} ]]; then
   rm ${2} 
    
fi

first=${1}
second=${2}
withoutId="$(cat $first | awk '{FS=","; OFS=","} {$1=""; print}' | sort | uniq)"

echo "$withoutId"

while read -r content; do
    echo $content
    echo "------A--------"
    matchedLines=$( cat ${first} | grep -E "$content")
    echo $matchedLines
    
    echo "$matchedLines" | sort -t ',' -k1 -n | head -n 1 >> $second 

done< <(echo "$withoutId")
