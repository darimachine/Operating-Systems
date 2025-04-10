#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 2
fi
if [[ ! -f ${2} ]]; then
    echo "second argument file doesnt exist"
    exit 3
fi

while read -r artist;do
    info=$(cat "${2}" | grep -E " ${artist}$")
    echo "; team ${artist}"
    while read -r composer; do
        
        while read -r hostname; do
            echo "${composer} IN NS ${hostname}.${1}."
        done< <(echo "${info}" | cut -d ' ' -f1)

    done< <(echo "${info}" | cut -d ' ' -f2)

done< <(cat "${2}" | tr -s ' ' | cut -d ' ' -f3 | sort | uniq)
