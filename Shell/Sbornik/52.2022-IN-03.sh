#!/bin/bash

if [[ ${1} != "java" ]]; then
    echo "incorrect command"
    exit 2
fi
shift 1
isJarFound=false
options=$(mktemp)
filename=""
arguments=$(mktemp)
for arg in ${@}; do
    if [[ -n ${filename} ]]; then
        echo "${arg}" >> "${arguments}"
        continue
    fi
    if [[ ${arg} =~ ^-jar$ ]]; then
        isJarFound=true
        continue
    fi
    if [[ ${isJarFound} == "false" ]] && [[ ${arg} =~ .*.jar$ ]]; then
        echo "Invalid Format"
        exit 3
    fi

    if [[ ${isJarFound} == "true" ]] && [[ ${arg} =~ .*.jar$ ]]; then
        filename=${arg}
        continue
    fi
    if [[ ${arg} =~ ^- ]]; then
        if [[ ${arg} =~ ^-D.*=.*$ ]] && [[ ${isJarFound} == "false" ]]; then
            # ne raboti zashtoto e predi opciqta JAR
            continue
        fi
        echo "${arg}" >> "${options}"
    fi


    echo "Args: ${arg}"
done
cmd=$(echo "$(cat "${options}" | tr -s '\n' ' ')-jar ${filename} $(cat "${arguments}" | tr -s '\n' ' ')")
echo "java ${cmd}"
java ${cmd}

rm ${options}
rm ${arguments}
