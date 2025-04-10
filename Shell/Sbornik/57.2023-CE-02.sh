#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -f ${1} ]]; then
    echo "File 1 doesnt exist"
    exit 2
fi

if [[ ! -f ${2} ]]; then
    echo "file 2 doesnt exist"
    exit 2
fi

first=$(cat "${1}" | grep -E "^${3}")
second=$(cat "${2}" | grep -E "^${3}")
distance1=$(echo "${first}" | cut -d ':' -f2 | awk '{print $1}')
distance2=$(echo "${second}" | cut -d ':' -f2 | awk '{print $1}')

if [[ -z ${first} ]] && [[ -z ${second} ]]; then
    echo "There is no black hole"
    exit 3
fi
if [[ -z ${first} ]]; then
    echo "${2}"
    exit 0
fi
if [[ -z ${second} ]]; then
    echo "${1}"
    exit 0
fi
if [[ ${distance1} -lt ${distance2} ]]; then
    echo "${1}"
    
    elif [[ ${distance2} -lt ${distance1} ]]; then
        echo "${2}"

    else
        echo "${1} ${2}"
fi
