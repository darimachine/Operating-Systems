#!/bin/bash
#if [[ $(id -u) != 0 ]]; then
#    echo "is not root"
#fi
userProccesInfo=$(mktemp)
ps -eo user=,rss=,pss= | awk '\

{rss[$1]+=$2} {counter[$1]+=1}

END {for(user in rss) {
    print user " " counter[user] " " rss[user]
}}
' > ${userProccesInfo}
cat ${userProccesInfo}

while read -r user count totalRss; do

    maxMemoryProces=$(ps -u ${user} -o rss=,pid= 2>/dev/null | sort -nr | head -n 1 )
    doubleMemory=$(echo "${totalRss}/${count}  * 2" | bc)
    echo "Highest Memory for ${user} memory:${maxMemoryProces}"
    echo "doubleTocheck: ${doubleMemory}"
    rssHighest=$(echo "${maxMemoryProces}" | awk '{print $1}')
    echo "TOP 1: ${rssHighest}"
    pidHighest=$(echo "${maxMemoryProces}" | awk '{print $2}')
    echo "TOP 1 PID: ${pidHighest}"
    if [[ ${rssHighest} -ge ${doubleMemory} ]]; then
        echo "prekrati na ${user} proces: ${pidHighest}"
        #kill pidHighest
    fi

done < <(cat ${userProccesInfo})

rm "${userProccesInfo}"
