#!/bin/bash
if [[ ${#} -ne 1 && ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Not directory"
    exit 2
fi

counter=0
while read -r line; do
    basename=$(basename ${line})
    pathToLink=$(readlink ${line})
    if [[ ! -e ${pathToLink} ]]; then
        counter=$(echo "${counter} + 1" |bc)
        continue
    fi

    if [[ -z ${2} ]]; then
        echo "${basename} -> ${pathToLink}"    
    else
        echo "${basename} -> ${pathToLink}" >> ${2}    
    fi
    
done< <(find ${1} -type l)

if [[ -z ${2} ]]; then
   echo "Broken symlinks: ${counter}"
else
   echo "Broken symlinks: ${counter}" >> ${2}
fi


