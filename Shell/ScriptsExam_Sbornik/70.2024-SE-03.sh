
#!/bin/bash
if [[ ${#} -ne 1 ]]; then
    echo "arg is missing"
    exit 2
fi

if [[ ! ${1} =~ ^[0-9]+ ]];then
    echo "arg must be positive number"
    exit 3
fi

N=$(echo ${1} % 12 | bc)
echo ${N}
tones=$(mktemp)
text=$(mktemp)
echo "A Bb" >> ${tones}
echo "Bb B" >> ${tones}
echo "B C" >> ${tones}
echo "C Db" >> ${tones}
echo "Db D" >> ${tones}
echo "D Eb" >> ${tones}
echo "Eb E" >> ${tones}
echo "E F" >> ${tones}
echo "F Gb" >> ${tones}
echo "Gb G" >> ${tones}
echo "G Ab" >> ${tones}
echo "Ab A" >> ${tones}

while read -r line; do    
    if [[ -z ${line} ]]; then
        break
    fi
    echo ${line} >> ${text}
done

akords=$(cat ${text} | grep -E -o "\[[^]]+\]" | sort | uniq)
#echo "${akords}"
mapToReplace=$(mktemp)
while read -r akord; do
    oneLetter=$(echo "${akord}" | cut -c 2)
    twoLetter=$(echo "${akord}" | cut -c 2-3)
    toneReplace=""
    counter=0
    accordTosave=""
    echo "${akord}"  | grep -q -E "^\[[BDEGA]b"
    if [[ ${?} -eq 0 ]]; then

        toneReplace=${twoLetter}
        #echo "Tone 2leter: ${toneReplace}"
        while true; do
            if [[ ${counter} -eq ${N} ]]; then
                break
            fi
            toneReplace=$(grep -E "^${toneReplace} " "${tones}" | cut -d ' ' -f2)
            counter=$((counter + 1))
        done
        #echo "ReplaceTone: ${toneReplace}"
        accordToSave="[${toneReplace}${akord:3}"
        #echo "AccordReplace: ${accordToSave}"
    else
        toneReplace=${oneLetter}
        #echo "Tone: ${toneReplace}"
        while true; do
            if [[ ${counter} -eq ${N} ]]; then
                break
            fi
            toneReplace=$(grep -E "^${toneReplace} " "${tones}" | cut -d ' ' -f2)
            counter=$((counter+1))
        done
        #echo "ReplaceTone: ${toneReplace}"
        accordToSave="[${toneReplace}${akord:2}"
        #echo "AccordRepalce: ${accordToSave}"

    fi
    echo "${akord} ${accordToSave}" >> ${mapToReplace}
done< <(echo "${akords}")

while read -r word toReplace; do
    cleanedWord=$(echo "${word}" | tr -d ']' | tr -d '[')
    cleanedReplace=$(echo "${toReplace}" | tr -d ']' | tr -d '[')
    #echo "Word: ${cleanedWord} ToRepalce: ${cleanedReplace}"
    sed -i -E "s:\[${cleanedWord}\]:\[${cleanedReplace}\]:g" ${text}
done< <(cat ${mapToReplace})
# 2 CHASA Shibana zadacha s tejko debugvane
cat ${text}
rm ${mapToReplace}
rm ${tones}
rm ${text}
