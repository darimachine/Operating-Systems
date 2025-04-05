#!/bin/bash

#if [[ $(id -u) -ne 1 ]]; then
 #   echo "You are not root"
  #  exit 2
#fi

fileName=${1}
result=$(cat ${fileName} | grep -E -v "home/")
while read -r row; do

    path=$(echo "${row}" | cut -d ':' -f6)
    if [[ -w ${path} ]]; then
        echo "${row}" >> result
    fi
    
done < <(cat ${fileName} | grep -E "home/" )

echo "${result}"
