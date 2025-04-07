#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "it must have 1 arg"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "file doesnt exist"
    exit 2
fi

while read -r siteName; do
    https=$(cat ${1} | grep -E "^.* ${siteName} -" | cut -d ' ' -f8)
    http2=$(echo "${https}" | grep -E "HTTP/2.0" | wc -l)
    non=$(echo "${https}" | grep -v -E "HTTP/2.0" | wc -l) 
    echo "${siteName} HTTP/2.0: ${http2} non-HTTP/2.0: ${non}"

done< <(cat ${1} | cut -d ' ' -f2 | sort | uniq -c | sort -nr | head -n 3 | sed -E "s:^ *::g" | cut -d ' ' -f2)

cat ${1} | awk -F " " ' $9>302 {print $1}' | sort | uniq -c
