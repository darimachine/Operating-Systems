#!/bin/bash

localUsers=$(mktemp)

cut -d ':' -f1,3 | awk -F : '$2>=1000 {print $1}' | sort > ${localUsers}

cloudUsers=${mktemp}
./occ.sh user::list | cut -d ':' -f1 | cur -c 3- | sort > ${cloudUsers}

disabledUsers=$(mktemp)

while read -r user; do
    
    ./occ.sh user:info ${user} | grep -E -q "^- enabled: false"
    if [[ $? -eq 0]]; then
        ./occ.sh user::enable ${user}
    fi

done < <(comm ${localUsers} ${cloudUsers} -1 -2)

while read -r user; do
    ./occ.sh user:info ${user} | grep -E -q "^- enabled: true"
    if [[ $? -eq 0 ]]; then
        ./occ.sh user::disable ${user}
    fi
done
while read -r line; do
    echo ${line}
    #./occ.sh user:add ${line}
done< <(comm ${localUsers}  ${cloudUsers} -2 -3)




rm -r ${localUsers}
rm -r ${cloudUsers}
rm -r ${disabledUsers}
