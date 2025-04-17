#!/bin/bash

isCorrect() {
    [[ $1 =~ ^[^0-9][a-zA-Z0-9_]*$ ]]
}

if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 2
fi
if [[ ! -d ${1} ]]; then
    echo "not directory"
    exit 3
fi

graph=$(mktemp)
notInside() {
    ! grep -q -E "^${1}$" "${graph}"
}
while read -r file; do
 className=$(cat "${file}" | cut -d ":" -f1 | cut -d ' ' -f2)
 if isCorrect ${className} && notInside ${className}; then
    echo "${className}" >> "${graph}" # ako ne e vutre go slagame
 fi
 descendant=$(cat "${file}" | awk -F : '$2!="" {print $2}' | tr ',' '\n')
 if [[ -z ${descendant} ]]; then
    continue
 fi
 while read -r mode parent; do
    if isCorrect ${parent} && notInside ${parent}; then
        echo "${parent}" >> "${graph}"
    fi
    echo "${parent} -> ${className}" >> "${graph}"
 done< <(echo "${descendant}" | sed -E "s:^ ::g")
 
done< <(find ${1} -mindepth 1 -type f -name "*.h" 2>/dev/null)
cat "${graph}" | sort -k2 > "${2}"
# dag-get ${graph} > ${2}
rm "${graph}"
