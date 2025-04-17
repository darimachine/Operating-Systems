#!/bin/bash

# priema 2 argumenta if check
# vunshna komanda nz kolko dulga
# put do fail
# izpulni komanda vzemi chislo vzemi tekushtoto vreme i go suhrani sushto
# v drugiq fail
# #? za posleden status na komanda i exit 3
if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 99
fi
currentTime=$(date +"%A %F %H")
weekDay=$(date +"%A")
hour=$(date +"%H")
value=$(${1})
if [[ ${?} -ne 0 ]]; then
    exit 3
fi

result=$(cat ${2} | grep -E "^${weekDay}" | \
awk -v hour=${hour} -F ' ' '$3 ~ /^hour/ {print $0}')

status=$(echo "${result}" | awk -v value=${value} -F ' ' '{sum+=$4}
END
{
    avg=sum/NR;
    if(value > 2*avg || value < avg/2) {
        print "Error its not correct"
    }
}
')
if [[ -n ${status} ]]; then
    echo "$(date +"%F %H"): ${value} abnormal"
    exit 2
fi

echo "${currentTime} ${value}" >> ${2}


