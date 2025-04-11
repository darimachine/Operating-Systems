#!/bin/bash
#myFileName=$(echo ".$(basename ${0}).swp")
#echo "${myFileName}"
if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi  
if [[ ! -d ${1} ]]; then
    echo "arg 1 should be directory"
    exit 2
fi

if [[ ! -d ${2} ]]; then
    echo "arg 2 should be directory"
    exit 3
fi
if [[ -n $(find ${2} -mindepth 1) ]]; then
    echo "dir 2 must be empty"
    exit 4
fi
cleanFiles=$(mktemp)
while read -r line; do
    dirName=$(dirname ${line} | sed -E "s:^[^/]*:${2}:g")
    echo "${dirName}" 
    mkdir -p ${dirName} # mkdir -p dir2 on the existing dir doesnt do anything
    
    echo "${line}" >> "${cleanFiles}"
    
done< <(find ${1} -mindepth 1 -type f ! -name "*.swp" 2>/dev/null)

while read -r line;do
    name=$(basename ${line} | cut -d '.' -f2)
    directory=$(dirname ${line})
    if ! grep -q -E "^${directory}/${name}$" "${cleanFiles}"; then
        echo "${line}" >> ${cleanFiles}
    fi
    #echo "${name}"
done< <(find ${1} -mindepth 1 -name "*.swp" 2>/dev/null)
cat "${cleanFiles}"

while read -r path;do
    dirPath=$(echo "${path}" | cut -d '/' -f2-)
    cp "${path}" "${2}/${dirPath}"
done< ${cleanFiles}

rm ${cleanFiles}


