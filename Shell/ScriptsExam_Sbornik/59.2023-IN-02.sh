#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "args mismatch"
    exit 2
fi

if [[ ! -d ${1} ]]; then
    echo "Not an directory"
    exit 3
fi

filesToDelete=$(mktemp)

shaSums=$(find ${1} -type f -printf "%n " -exec sha256sum {} ';')
echo "${shaSums}"
while read -r uniqHash; do
    
    group=$(echo "${shaSums}" | grep -E "${uniqHash}" | tr -s ' ' | sort -n)
    echo "----------------"
    echo "${group}"
    if [[ $(echo "${group}" | wc -l) -eq 1 ]]; then
        continue
    fi
    linkCount=$(echo "${group}" | cut -d ' ' -f1 | head -n 1)
    if [[ $(echo "${group}" | grep -E "^${linkCount}" | wc -l) -eq $(echo "${group}" | wc -l) ]]; then
        #popadame v purvite 2 sluchaq ili sa samo failove ili samo hardlink
        if [[ $(echo ${linkCount}) -eq 1 ]]; then
            ## samo failove
            while read -r link hash filename;do
                echo "${link} ${hash} ${filename}"
                echo "${filename}" >> ${filesToDelete}
            done< <(echo "${group}" | tail -n +2 )

        else
            #samo hardlinkove shte mahna purviq
            echo "${group}" | cut -d ' ' -f3 | head -n 1 >> ${filesToDelete}
        fi
    else
        # kombinaciq mejdu normal i hardlink
        #mahame vsichki normalni
        while read -r link hash filename;do
            echo "${filename}" >> ${filesToDelete}
        done< <(echo "${group}" | grep -E "^1")
        #maheme purviq koito ne pochva s 1  
        echo "${group}" | grep -E -v "^1" | cut -d ' ' -f3 | head -n 1 >> ${filesToDelete}
    fi

done< <(echo "${shaSums}" | tr -s ' ' | cut -d ' ' -f2 | sort | uniq)

cat ${filesToDelete}

rm ${filesToDelete}
