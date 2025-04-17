#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "Arg mismatch"
    exit 1
fi
if [[ ! -d ${1} ]]; then
    echo "first dir must exist"
    exit 2
fi

if [[ -d ${2} ]]; then
    rm -r ${2}
fi
mkdir ${2}
#find na src i .jpg

while read -r file; do
    
    echo "FILE......: ${file}"
    cleaned=$(basename "${file}" | tr -s ' ' | sed -E "s:^ +::g" | sed -E "s: +$::g"\
    | sed -E "s:.jpg::g")
    echo "Cleaned: ${cleaned}"
    title=$(echo "${cleaned}" | sed -E "s:\([^\)]*\)::g" | tr -s ' ' | sed -E "s: +$::g" \
    | sed -E "s:^ +::g")
    echo "Title : ${title}"
    album=$(echo ${cleaned} | sed -E "s:^.*\((.*)\)[^\(\)]*$:\1:g" | sed -E "s:^${cleaned}::g")
    if [[ -z ${album} ]]; then
        album="misc"
    fi
    echo "Album: ${album}"
    date=$(stat -c "%y" "${file}" | cut -d ' ' -f1)
    echo "Date: ${date}"
    sha256=$( sha256sum "${file}" | cut -c 1-16)
    echo "Sha256: ${sha256}"

    mkdir -p "${2}/images"
    mkdir -p "${2}/by-date/${date}/by-album/${album}/by-title"
    mkdir -p "${2}/by-date/${date}/by-title"
    mkdir -p "${2}/by-album/${album}/by-date/${date}/by-title"
    mkdir -p "${2}/by-album/${album}/by-title"
    mkdir -p "${2}/by-title"
        
    cp -r "${file}" "${2}/images/${sha256}.jpg"
    ln -s "${2}/images/${sha256}.jpg" "${2}/by-date/${date}/by-album/${album}/by-title/${title}.jpg"
    ln -s "${2}/images/${sha256}.jpg" "${2}/by-date/${date}/by-title/${title}.jpg"
    ln -s "${2}/images/${sha256}.jpg" "${2}/by-album/${album}/by-date/${date}/by-title/${title}.jpg"
    ln -s "${2}/images/${sha256}.jpg" "${2}/by-album/${album}/by-title/${title}.jpg"
    ln -s "${2}/images/${sha256}.jpg" "${2}/by-title/${title}.jpg"

done< <(find ${1} -type f -name "*.jpg")
