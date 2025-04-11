#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "args mismatch"
    exit 2
fi
if [[ ! ${1} =~ [0-9]+\.?[0-9]* ]]; then
    echo "args 1 is not number"
    exit 3
fi

if [[ ! ${2} =~ [a-zA-Z] ]]; then
    echo "expected Letter as 2nd arg!"
    exit 4
fi

if [[ ! ${3} =~ [a-zA-Z] ]]; then
    echo "expected Letter as 3nd arg!"
    exit 5
fi

decimal=$(cat "prefix.csv" | grep -E ",${2}," | cut -d ',' -f3 )
decimal2=$(cat "prefix.csv" | awk -F ',' -v prefix=${2} '$2==prefix {print $3}')
echo "Decimal Grep : ${decimal} Decimal Awk: ${decimal2}"
measure=$(cat "base.csv" | grep -E ",${3}," | cut -d ',' -f3)
unit_name=$(cat "base.csv" | grep -E ",${3}," | cut -d ',' -f1)

converted=$(echo "scale=1; ${1} * ${decimal}" | bc)
echo "${converted} ${3} (${measure}, ${unit_name})"

