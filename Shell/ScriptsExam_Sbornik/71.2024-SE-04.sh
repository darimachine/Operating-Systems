#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "It must have 1 argument"
    exit 1
fi
currDepend=${1}
bakefile="bakefile"
if ! grep -E -q "^${1}" "${bakefile}"; then
    if [[ ! -f ${1} ]]; then
        echo "Error we cannot continue"
        exit 2
    fi
    echo "Izgraden"
    exit 0
    
    else
        zavisimost=$(cat ${bakefile} | grep -E "^${currDepend}:" | cut -d ':' -f2)

        isNewer=false
        for curr in ${zavisimost}; do
            if [[ ${curr} -nt ${currDepend} ]]; then
                isNewer=true
            fi
            ./71.2024-SE-04.sh "${curr}"
        done
        if [[ ${isNewer} == true ]]; then
            command=$(cat ${bakefile} | grep -E "^${currDepend}:" | cut -d ':' -f3)
            eval command
        fi
fi

