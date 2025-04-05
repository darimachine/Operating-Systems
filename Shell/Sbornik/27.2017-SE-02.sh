#/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "Error args must be 3"
    exit 1
fi
if [[ ! -d ${1} || ! -d ${2} || $(find ${2} -type f | wc -l) -gt 0 ]]; then
    echo "arg 1 or 2 is not directory or arg 2 contains files"
    exit 2
fi
#if [[ $(id -u) != 0 ]]; then
 #echo "You are not root"
#fi
#find na src imat podniz *ABC* i da gi mesti v DST
while read -r path; do
    baseName=$(basename ${path})
    dr=$(dirname ${path})
    echo "BaseName: ${baseName}"
    echo "Dirname: ${dr}"
    editedDir=$( echo ${dr} | sed -E s:${1}:${2}:g )
    mkdir -p ${editedDir}
    cp ${path} "${editedDir}/"
   

done< <(find ${1} -type f -name "*${3}*")
