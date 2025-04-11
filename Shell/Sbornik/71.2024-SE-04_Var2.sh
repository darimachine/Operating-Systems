#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -f "bakefile" ]]; then
    echo "bakefile doesnt exits"
    exit 2
fi
recursion() {
    echo "Arg: ${1}"    
    row=$(cat "bakefile" | grep -E "^${1}")
    echo "Row: ${row}"
    if [[ -z ${row} ]]; then
        if [[ ! -f ${1} ]]; then
            echo "Error you cannot continue"
            exit 3
        fi

    else
        zavisimosti=$(echo "${row}" | cut -d ':' -f2)
        echo "Zavisimost: ${zavisimosti}"
        cmd=$(echo "${row}" | cut -d ":" -f3-)
        if [[ -z ${zavisimosti} ]] || [[ -z  ${cmd} ]]; then
            #format check
            echo "Eror no dependency or command"
            exit 5
        fi
        for dep in ${zavisimosti}; do
            echo "FOr ${dep}"
            recursion "${dep}"
        done
        skip="true" # to check whether file building(command execution should be skipped)
        if [[ -f "${1}" ]]; then
            
            for dep2 in ${zavisimosti}; do
                    if [[ ${dep2} -nt ${1} ]]; then
                        skip="false"
                        break
                    fi
            done
        fi
        if [[ ${skip} == "false" ]]; then
            eval "${cmd}"
            echo "Which comm ${cmd}"
        fi

    fi

}

recursion ${1}
