#!/bin/bash
#args check
# directory check
# find moje bi
if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -d ${1} ]]; then
    echo "Is not directory"
    exit 2
fi

regRes=$(mktemp)
while read -r line; do
    #echo "${line}"
    name=$(basename ${line})
    #echo "${name}"
    if [[ ${name} =~ ^vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${2} ]]; then
        echo "${name}" >> ${regRes}
    fi
done< <(find ${1} -maxdepth 1 -type f )

cat ${regRes} | sort -t '.' -k1 -k2 -k3 -nr | head -n 1
rm ${regRes}
#or
find ${1} -maxdepth 1 -type f -printf "%f\n" | grep -E "^vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${2}" | sort -t '.' -k1 -k2 -k3 -nr | head -n 1
