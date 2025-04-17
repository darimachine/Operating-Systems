#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "args mismatch"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Must be directory"
    exit 2
fi

friendRowMessage=$(mktemp)
while read -r file; do
    logName=$(basename ${file})
    rowCount=$(cat ${file} | wc -l)
    dirName=$(dirname ${file})
    friend=$(echo "${dirName}" | awk -F / '{print $NF}')
    echo "${friend} ${rowCount}" >> ${friendRowMessage}

done< <(find . -type f -name "*.txt")
cat "${friendRowMessage}" #works perfect
totalMessageWithFriends=$(mktemp)
echo "----------------------------"
cat "${friendRowMessage}" | awk 'BEGIN {friend[$1]=0} {friend[$1]+=$2} \
END \
{
    for(fr in friend)  
    {
        print fr " " friend[fr]
    }

}
' >> ${totalMessageWithFriends}
cat ${totalMessageWithFriends} | sort -t ' ' -k2 -nr | head -n 10

rm ${friendRowMessage}
rm ${totalMessageWithFriends}


