#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -d ${1} ]]; then
    echo "Not directory"
    exit 2
fi
wordCountFile=$(mktemp)
allWordCount=$(mktemp)
fileCount=$(find ${1} -type f | wc -l)
while read -r file; do
    words=$(cat "${file}" | grep -E -o "[a-z]*")
    echo "${words}" | sort | uniq -c | awk '$1 > 3 {print $2}' >> ${wordCountFile}
    
    #MAHNI TOZI WHILE I IMASH RESHENIE NA PURVATA POTOCHKA A VTORATA SE RESHAVA S MAP!!!!
    while read -r word; do
        #echo "Without Cleaned: ${word}"
        cleaned=$(echo "${word}" | tr -s ' ' | cut -c 1-)

        #echo "Cleaned: ${cleaned}"
        currentWord=$(echo "${cleaned}" | cut -d ' ' -f2)
        currentOcc=$(echo ${cleaned} | cut -d ' ' -f1)
        #echo "${cleaned}"
        if grep -q -E "^${currentWord} [0-9]*$" "${allWordCount}"; then
            wordOcc=$(cat ${allWordCount} | grep -E "${currentWord}" | cut -d ' ' -f2) # pazq (duma cifra)
            totalOcc=$(echo "${wordOcc} + ${currentOcc}" | bc)
            # shibaniq sed ne raboti kakto trqbva nz shto ideqta e da implementiram map za vsichki dumi ama ne stana
            
            sed -E -i "s:^${currentWord} [0-9]*$:${currentWord} ${totalOcc}:g" "${allWordCount}"
            #echo "exit code: $? Word: ${currentWord} total: ${totalOcc}"
            continue
        fi
        echo "${currentWord} ${currentOcc}" >> ${allWordCount}
        
    done< <(echo "${words}" | sort | uniq -c)
done< <(find ${1} -type f -name "*.txt")
cat ${wordCountFile} | sort | uniq -c | awk -v minFile=${fileCount} '$1 > (minFile/2) {print $2}'
echo "---------------------------------------------------------------"
#echo "Filecount: $fileCount"
cat ${allWordCount}
rm ${wordCountFile}
rm ${allWordCount}
