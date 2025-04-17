#!/bin/bash
isnumber() {
 input=${1}
 if [[ $input =~ ^-?[1-9]+[0-9]*$ ]]; then
    return 0

 else
    return 1
 fi
 
}

file=${1}
numbers=()
while read -r input; do
   # echo "$input"
    if isnumber $input; then
        numbers+=("$input")
    #else
       # echo "Its not Number"
    fi

done< <(cat $file)

numbersArr=$(echo "${numbers[@]}" | tr ' ' '\n')
#echo "$numbersArr"
maxNum=$(echo "${numbersArr}"| tr -d '-'| sort -nr| head -n 1)

echo "${numbersArr}" | sort | uniq | egrep "^-?${maxNum}"
echo "b()--------------------"
echo "${numbersArr}"

maxDigitSum=$(echo "$numbersArr" | tr -d '-' |awk ' BEGIN { mVal=0 } {sum=0}{
for(i=1;i<=length($1);i++) \
{
    sum+=substr($1,i,1)
}
if(sum>mVal) mVal=sum
}
END {print mVal}
')

echo "Result:"
echo "$numbersArr" | sort -n | awk -v m="$maxDigitSum" '{sum=0; for(i=1;i<=length($1);i++) {
sum +=substr($1,i,1)

}
if(sum==m) {print $1; exit}
}'


#OR
#tmpFile=$(mktemp)
#maxDig=0
#while read -r num; do
#    sum=$( echo "$num" | tr -d '-' |egrep -o "[0-9]" | awk '{sum+=$1} END {print sum}')
#    echo "${num} ${sum}" >>$tmpFile
#        
#    done< <(echo "$numbersArr")
#echo "llllllllllllllllllllllll"
#echo "$maxDig"
#
#cat "$tmpFile" | sort -t ' ' -k2,2nr -k1,1n | head -n 1 | cut -d ' ' -f1
#rm $tmpFile
#








