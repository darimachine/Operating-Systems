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
stopWordsFinal=$(mktemp)
fileCount=$(find ${1} -type f | wc -l)
echo "Kolko sa: ${fileCount}"
while read -r file; do
    words=$(cat "${file}" | grep -E -o "[a-z]+")
    echo "${words}" >> ${allWordCount}
    echo "${words}" | sort | uniq -c | awk '$1 >= 3 {print $2}' >> ${wordCountFile}
    
done< <(find ${1} -type f -name "*.txt")
cat ${wordCountFile} | sort | uniq -c | sort -nr | awk -v minFile=${fileCount} '$1 >minFile/2 {print $2}' | head -n 10 >>${stopWordsFinal}

result=$(mktemp)
cat ${allWordCount}
while read -r word; do
    #echo "${word}"
    cat ${allWordCount} | sort | uniq -c | sort -nr | grep -E "\b${word}\b" | tr -s ' ' | sed -E "s:^ ::g" >>${result}
done < <(cat ${stopWordsFinal})
echo "---------------------------------------------------------------"
#echo "Filecount: $fileCount"
cat ${result} | sort -nr

rm ${result}
rm ${stopWordsFinal}
rm ${wordCountFile}
rm ${allWordCount}
