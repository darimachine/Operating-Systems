#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You must have 1 positional argument"
    exit 1
fi
#check root privige
#if [[ $(id -u) != 1 ]]; then
 #   echo "is not root"
  #  exit 2
#fi
isNumber(){

   [[ $1 =~ ^[1-9]+[0-9]*$ ]]
}

if ! isNumber $1 ; then
    echo "Its not number"
fi

groupBy=$(ps -eo user=,rss=,pss= | awk -v "maxNum=${1}" '{users[$1]+=$2} END {for(user in users) {\
    if(users[user] > maxNum) {
        print user " " users[user]
    }

}


}')

while read -r row; do
    user=$(echo "${row}" | awk '{print $1}')
   # echo "${user}"
    sum=$(echo "${row}" | tr -s ' '|cut -d ' ' -f2)
    #echo "${sum}"
    pid=$(ps -u "${user}" -o user=,pid=,rss=,pss= | sort -k3nr | head -n 1 | tr -s ' ' | cut -d ' ' -f2)
    echo "${pid}"
    kill "${pid}"

done< <(echo "${groupBy}")
