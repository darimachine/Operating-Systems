#!/bin/bash

# first args chislo v sekundi
# ostanaloto komanda i argumenti
if [[ ! ${#} -gt 1 ]]; then
    echo "arg mismatch"
    exit 1
fi
if [[ ! ${1} =~ ^[1-9]+[0-9]* ]]; then
    echo "first arg is not number"
    exit 2
fi
sec=${1}
allSec=0
shift 1
counter=0
while [[ $(echo "${sec} > ${allSec}" | bc) -eq 1 ]];  do
    startTime=$(date "+%s.%N" | cut -c -13)
    $(echo "${@}") &> /dev/stdout
    endTime=$(date "+%s.N" | cut -c -13)
    secondToTake=$(echo "scale=2;${endTime} - ${startTime}" | bc)
    allSec=$(echo "${secondToTake} + ${allSec}" | bc )
    echo "${allSec}" 
    counter=$(echo "${counter}+1" | bc)
done
echo "It has be run ${counter} times total time: ${allSec}"
echo "Average runtime: $(echo "scale=2;${allSec} / ${counter}" | bc) seconds"
