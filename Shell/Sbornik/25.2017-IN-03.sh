#!/bin/bash

lastModified=$(find /home/students -type f -printf "%u %p %T@ %f\n" 2>/dev/null | sort -k3 -n -t " " | tail -n 1)
echo ${lastModified}
name=$(echo "${lastModified}" | cut -d ' ' -f1)
echo "Username: ${name}"
fileName=$(echo "${lastModified}" | cut -d ' ' -f4)
echo "Filename: ${fileName}"
