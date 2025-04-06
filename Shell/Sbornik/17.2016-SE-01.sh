#!/bin/bash

if [[ $# != 1 ]]; then
    echo "Wrong Arguments"
    exit 1
fi
par=${1}
if [[ -d $par ]]; then

    find $par -type l
else
    echo "The argument is not directory"
fi
