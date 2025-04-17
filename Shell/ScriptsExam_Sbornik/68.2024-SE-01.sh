#/bin/bash

if [[ ${#} -lt 2 ]]; then
    echo "args should be more than 1"
    exit 1
fi

if [[ ${!#} =~ ^- ]]; then
    echo "file shouldnt start with -"
    exit 3
fi

if [[ ! -f ${!#} ]]; then
    echo "File doesnt exist"
    exit 2
fi

filename=${!#}
#echo "${filename}"
replaceWord=$(mktemp)
randomKey=$(pwgen | cut -d ' ' -f1)
for arg in "${@:1:$#-1}"; do
    
    if [[ ! ${arg} =~ ^-R[A-Za-z0-9]+=[A-Za-z0-9]+$ ]]; then
        echo "Incorrect argument"
        exit 2
    fi
    firstWord=$(echo "${arg}" | cut -d '=' -f1 | cut -c 3-)
    secondWord=$(echo "${arg}" | cut -d '=' -f2)
    echo "${firstWord} ${secondWord}" >> ${replaceWord}
done
cat ${replaceWord}
while read -r key value; do
    sed -i -E "/^#/! s:\b${key}\b:${value}${randomKey}:g" ${filename}
done< <(cat "${replaceWord}")

sed -i -E "s:${randomKey}::g" ${filename}


