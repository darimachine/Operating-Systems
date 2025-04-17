#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi
if [[ ! -d ${1} || ! -d ${2} ]]; then
    echo "dirst must exist"
    exit 2
fi

packageVersion=$(cat ${2}/version)
echo "PackageVersion: ${packageVersion}"
packages=$(cat ${1}/db)
name=$(basename ${2})

if grep -q "${name}-${packageVersion}" ${1}/db; then
    
    hashSum=$(echo "${packages}" | grep -E "${name}-${packageVersion}" | cut -d ' ' -f2)
    echo "${hashSum}"
    rm -r "${1}/packages/${hashSum}.tar.xz"
    tar -caf arhiv.tar.xz ${2}/tree
    shaSum=$(sha256sum arhiv.tar.xz | cut -d ' ' -f1)
    mv arhiv.tar.xz "${1}/packages/${shaSum}.tar.xz"
    
    grep -E "^([^-]*-${packageVersion}) .*$" "${1}/db" 
    sed -E -i "s:^([^-]*-${packageVersion}) .*$:\1 ${shaSum}:g" "${1}/db"
else
    tar -caf arhiv.tar.xz ${2}/tree
    shaSum=$(sha256sum arhiv.tar.xz | cut -d ' ' -f1)
    mv arhiv.tar.xz ${1}/packages/${shaSum}.tar.xz
    temp=$(mktemp)
    cat ${1}/db > ${temp}
    echo "${name}-${packageVersion} ${shaSum}" >> ${temp}
    cat ${temp} | sort > ${1}/db
    rm ${temp}
fi

