#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "args must be 2"
    exit 2
fi

if [[ ! -f ${1} ]]; then
    echo "file 1 doesnt exist"
    exit 4
fi

if [[ ! ${1} =~ .*.csv ]]; then
    echo "must be csv file"
    exit 3
fi

# imeto na nay qrkata zvezda ot suzvezdieto, v koeto ima 
# nai mnogo zvezdi ot tipa podaden kato vtori parametur
typee=$(cat "${1}" | grep -E "^[^,]*,[^,]*,[^,]*,[^,]*,${2}.*$") # moje s awk po lesno
suzvezdie=$(echo "${typee}" | cut -d ',' -f4 | sort | uniq -c | sort -nr | head -n 1 | awk '{print $2}')
suzvezdieNaiMnogo=$(echo "${typee}" | grep -E "^[^,]*,[^,]*,[^,]*,${suzvezdie}.*$") # moje s awk po lesno


naiQrka=$(echo "${suzvezdieNaiMnogo}" | sort -t ',' -k7n | head -n 1)

echo "${naiQrka}" | cut -d "," -f1
#echo "${suzvezdie}"
