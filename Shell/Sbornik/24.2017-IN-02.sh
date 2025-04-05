#!/bin/bash
# 1 args valid
# ps -u Foo -eo
if [[ ${#} -ne 1 ]]; then
    echo "must have 1 args"
    exit 2
fi

#if [[ $(id -u) -ne 1 ]]; then
 #   exit 2
#done
currUserProcesses=$(ps -u ${1} -o pid= | wc -l)
#echo "${currUserProcesses}"
ps -eo user= | sort | uniq -c | sort -rn | awk -v max=${currUserProcesses} '$1 > max {print $2}'

avgTime=$(ps -eo user=,pid=,cputimes= | awk 'BEGIN {allSeconds=0} {allSeconds+=$3} END {printf "%.3f", allSeconds/NR }')
echo "${avgTime}"
limit=$(echo "scale=2; ${avgTime} * 2" | bc)
echo "${limit}"
while read -r pid time; do
    echo "PID : ${pid}"
    echo "TIME: ${time}"
    if [[ $(echo "${time} > ${limit}" | bc) -eq 1 ]]; then
    
        kill ${pid}
    fi
done< <(ps -u ${1} -o pid=,cputimes=)
