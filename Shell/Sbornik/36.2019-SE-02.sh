#!/bin/bash
if [[ ${#} -eq 0 ]]; then
    echo "Expected args"
    exit 1
fi
N=10
if [[ ${1} == "-n" ]]; then
    if [[ ! ${2} =~ ^[1-9]+[0-9]*$ ]]; then
        echo "Second args must be number"
        exit 2
    fi
    N=${2}
    shift 2
fi
allDates="$(mktemp)"

for file in "${@}"; do
    idf=$(basename ${file} | sed -E s:".log"::g)
    while read -r line; do
        info=$(echo "${line}" | cut -d ' ' -f3-)
        date=$(echo "${line}" | cut -d ' ' -f1,2)
        echo "${date} ${idf} ${info}" >> ${allDates}
    done< <(cat ${file})
done
cat ${allDates} | sort -t " " -k1 -k2 | tail -n ${N}
rm ${allDates}
