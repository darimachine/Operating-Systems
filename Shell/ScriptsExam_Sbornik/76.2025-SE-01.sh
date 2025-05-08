#!/bin/bash
# Solution for max points 15/15

if [[ ${#} -ne 2 ]];then
    echo "args must be 2"
    exit 1
fi
if [[ ! -f ${1} ]]; then
    echo "file 1 doesnt exist"
    exit 2
fi
if [[ ! -f ${2} ]]; then
    echo "file 2 doesnt exist"
    exit 3
fi
# dopulnitelni bonus proverki
if [[ ${1} =~ ^- ]]; then
    echo "file 1 ${1} cannot start with -"
    exit 4
fi
if [[ ${2} =~ ^- ]];then
    echo "file 2 ${2} cannot start with -"
    exit 5
fi
if [[ ! ${1} =~ [:alnum:] ]]; then
    echo "file 1 contains special characters Error"
    exit 6
fi
if [[ ! ${2} =~ [:alnum:] ]]; then
    echo "file 2 contains special characters Error"
    exit 7
fi
commands=$(mktemp)
while read -r line; do
    language=$(echo "${line}" | tr -s ' ' |cut -d ' ' -f1) # tr -s ' ' bonus
    echo "Language: ${language}"
    options=$(echo "${line}" | tr -s ' ' |cut -d ' ' -f2- | sed -E "s:([^']*).*$:\1:g" | sed -E "s: $::g") # same
    
    echo "Options:${options}"
    basedirectory=$(echo "${line}" | cut -d ' ' -f2- | sed -E "s:[^']*(.*)$:\1:g"| tr -d "'")
    echo "Directory:${basedirectory}"
    baseFileName=$(basename "${2}" | sed -E "s:(.*)\.[^\.]*$:\1:g ") # finding the last point
    echo "baseFileName: ${baseFileName}"
    if ! $(echo "${options}" | grep -q -E "recognizer"); then
      echo "Recognizer not found"
      exit 4
    fi
    optionsToParse=$(mktemp)
    mkdir -p "${basedirectory}/${baseFileName}"
    if ! $(echo "${options}" | grep -q -E "listener"); then
        echo "-no-listener" >> ${optionsToParse}
    fi
    
    if $(echo "${options}" | grep -q -E "visitor"); then
        echo "-visitor" >> ${optionsToParse}
    fi
    cleanOptions=$(cat "${optionsToParse}" | tr '\n' ' ')
    #test scenario
    echo "-Dlanguage=${language} ${cleanOptions}-o ${basedirectory}/${baseFileName} ${2}"
    
    echo "-Dlanguage=${language} ${cleanOptions}-o ${basedirectory}/${baseFileName} ${2}" >> ${commands}
    
    rm ${optionsToParse}
    echo "Line: ${line}"
done< <(cat ${1})

while read -r myComm; do
    echo "antlr4 ${myComm}"
    eval "antlr4 ${myComm}"
done< <(cat "${commands}")
rm ${commands}
