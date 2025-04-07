#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi

if [[ ! -f  ${1} ]]; then
    echo "file doesnt exist"
    exit 2
fi
if [[ ! -d ${2} ]]; then
    echo "directory doesnt exist"
    exit 3
fi

isWord() {
    [[ $1 =~ [a-zA-Z0-9_] ]]
}

while read -r file; do
    
    while read -r word; do
        echo "Word: ${word}"
        censorer=$(echo ${word} | sed -E 's:.:\*:g')
        echo "Censore: ${censorer}"
        caseWord=$(cat "${file}" | grep -E -o -i " ${word} " | tr -d ' ')
        while read -r change; do 
           sed -i -E "s: ${change} : ${censorer} :g" "${file}"
        done< <(echo "${caseWord}")

    done< <(cat ${1})
done< <(find ${2} -type f -name "*.txt" 2>/dev/null)

# find with name .txt

