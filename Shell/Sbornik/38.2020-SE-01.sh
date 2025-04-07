#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "args mismatch"
    exit 1
fi

if [[ ! -d ${2} ]]; then
    echo "Directory doesnt exist"
    exit 3
fi
if [[ ! -f ${1} ]]; then
   touch ${2}/${1}
fi

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" >${1}

while read -r file; do
    hostname=$(basename ${file} | sed -E s:".log$"::g)
    echo "Hostname: ${hostname}"
    phy=$(cat ${file} | grep -E "Maximum Physical Interfaces" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Phy: ${phy}"
    vlans=$(cat ${file} | grep -E "VLANs" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Vlans: ${vlans}"
    hosts=$(cat ${file} | grep -E "Inside Hosts" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Hosts: ${hosts}"
    failover=$(cat ${file} | grep -E "Failover" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Failover: ${failover}"
    vpn3des=$(cat ${file} | grep -E "VPN-3DES" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "VPN-3DES: ${vpn3des}"
    peers=$(cat ${file} | grep -E "Total VPN Peers" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Peers: ${peers}"
    trunkPort=$(cat ${file} | grep -E "VLAN Trunk Ports" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "VLAN Trunk Ports: ${trunkPort}"
    license=$(cat ${file} | grep -E "license." | cut -d ':' -f2 | sed -E s:"^This platform has [^ ]+ (.*) license\.$:\1:g")
    echo "License: ${license}"
    sn=$(cat ${file} | grep -E "Serial Number:" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Serial Number: ${sn}"
    key=$(cat ${file} | grep -E "Running Activation Key:" | cut -d ':' -f2 | sed -E s:"^ "::g)
    echo "Key: ${key}"
    echo "${hostname},${phy},${vlans},${hosts},${failover},${vpn3des},${peers},${trunkPort},${license},${sn},${key}" >> ${1}
done< <(find ${2} -type f -name "*.log")


