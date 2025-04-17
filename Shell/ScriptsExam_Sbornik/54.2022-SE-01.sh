#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "arg mismatch"
    exit 1
fi
if [[ ! -f ${1} ]]; then
    echo "it must be file"
fi
procFile="wakeupExample"

cleanedFile=$(mktemp)

while read -r line; do
    if [[ -z ${line} ]]; then
        continue
    fi
    clearLine=$(echo "${line}" | tr -s ' ')
    device=$(echo "${line}" | awk '{print $1}')
    echo "${device}"
    if [[ ${#device} -gt 4 || ! ${device} =~ ^[A-Z0-9]*$ ]]; then
        echo "Device doesnt meet criteria: ${device}"
        continue
    fi
    status=$(echo "${line}" | awk '{print $2}')
    echo "Status ${status}"
    if [[ ! ${status} =~ ^(enabled|disabled)$ ]]; then
        echo "Status must be enabled or disabled ${status}"
        continue
    fi
    echo "${device} ${status}" >> ${cleanedFile}
done< <(cat ${1} | sed -E "s:^([^#]*)#.*$:\1:g")

while read -r line; do
    currDevice=$(echo "${line}" | cut -d ' ' -f1)
    currStatus=$(echo "${line}" | cut -d ' ' -f2)
    echo "CURR DEVICE: ${currDevice} CURR STATUS : ${currStatus}"    
    if ! grep -q -E "${currDevice}" ${procFile}; then
        echo "There is no device with this name ${currdevice}"
        continue
    fi
    lineToChange=$(cat ${procFile} | grep -E "${currDevice}")
    echo "${lineToChange}"
    lineStatus=$(echo "${lineToChange}" | awk '{print $3}' | tr -d '*')
    echo "STATUS TO CHECK: ${currDevice} PEDAL ${lineStatus} != ${currStatus}"
    if [[ "${lineStatus}" != "${currStatus}" ]]; then
        echo "Replacin--------------- ${currDevice}"
        sed -E -i "s:^(.*${currDevice}.*)\*${lineStatus}(.*)$:\1\*${currStatus}\2:" "${procFile}"
    fi
done< <(cat ${cleanedFile})



rm ${cleanedFile}
