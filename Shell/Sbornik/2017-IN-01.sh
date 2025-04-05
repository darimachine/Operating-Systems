#!/bin/bash

# 3 args mandatory
# check first argument  file exist
# comm za termone
if [[ ${#} -ne 3 ]]; then
    echo "args should be 3"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "file doesnt exist"
    exit 2
fi

secondKey=$(grep -E "^${3}=" ${1})
if [[ ${?} -ne 0 ]]; then
     cat "${1}"
    exit 3
fi
firstKeyArgs=$(cat ${1} | grep -E "^${2}="| cut -d '=' -f2- | grep -o .)
secondKeyArgs=$(echo "${secondKey}" | cut -d '=' -f2- | grep -o .)

result=$(comm -1 -3 <(echo "${firstKeyArgs}" | sort) <( echo "${secondKeyArgs}" | sort) | tr '\n' ' ')
#cat ${1} | sed -E  "s:(^${3}).*:\1=${result}:g" > newProba
sed -E -i  "s:(^${3}).*:\1=${result}:g" ${1}
