#!/bin/bash

#if [[ ! $(id -u) eq 0 ]]; then

#fi
alreadyCheckedUsers=$(mktemp)
maxMemoryRoot=$(ps -u root -o rss= | awk '{sum+=$1} END {print sum}')
while read -r pid user rss; do
    
    
    if grep -q -E "${user}" ${alreadyCheckedUsers}; then
        continue
    fi
    homeDir=$(cat /etc/passwd | grep ${user} | cut -d ':' -f6)
    if [[ -z ${homeDir} ]]; then
        continue
    fi  
    ownerOfHomedir=$(stat -c "%U" ${homeDir} )
    permissions=$(stat -c "%A" ${homeDir} )
    echo "Homedir: ${homeDir}"
    echo "OwnerOfHomerdir: ${ownerOfHomedir}"
    echo "Permissions: ${permissions}"
    echo "Current User: ${user}"
    if [[ ! -d ${homeDir} || ${user} != ${ownerOfHomedir} || ${permissions:2:1} != "w" ]]; then
        currentUserMemory=$(ps -u ${user} -o rss=| awk '{sum+=$1} END {print sum}')
        echo "---------------------------------VLEZNAH: ${user} sus ${currentUserMemory} > ${maxMemoryRoot}"
        if [[ ${currentUserMemory} -gt ${maxMemoryRoot} ]]; then
            echo "Ubiy Usera: ${user}"
            #killall -u ${user}
        fi
    fi
    echo "${user}" >> ${alreadyCheckedUsers}
    #cat ${alreadyCHeckeedUsers}
done< <(ps -u root -o pid=,user=,rss= -N)

rm ${alreadyCheckedUsers}
