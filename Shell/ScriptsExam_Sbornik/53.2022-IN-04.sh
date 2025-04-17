#!/bin/bash
if [[ ${#} -ne 1 ]];then
    echo "args mismatch"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "directory doesnt exist"
    exit 2
fi
>foo.conf # izchistvame sudurjanieto
while read -r line; do
    name=$(basename ${line})
    row=$(./validate.sh ${line})
    ec=${?}  # IMPORTANT OR EVERY COMMAND CHANGES THE EXIT CODE AND DOEST WORK
    
    if [[ "${ec}" -eq 1 ]]; then
        
       echo "${name}:$(./validate.sh ${line})" >/dev/stderr

    elif [[ "${ec}" -eq 0 ]]; then # then its valid row
        
        cat ${line} >> foo.conf
        cleanName=$(echo "${name}" | sed -E "s:.cfg$::g")
        if ! cat "foo.pwd" | grep -q -E "^${cleanName}:"; then
            password=$(pwgen 20 1)
            hashedPw=$(mkpasswd "${pasword}")
            echo "${cleanName}:${hashedPw}" >>"foo.pwd"
            echo "${cleanName}:${hashedPw}"
        else
            echo "User: ${cleanName} already in foo.pwd"
        fi
    else
        echo "Error running script"
    fi
done< <(find "${1}/cfg" -mindepth 1 -type f 2>/dev/null)

