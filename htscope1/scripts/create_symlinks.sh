#!/bin/bash

# Usage: ./scripts/create_links.sh $USER

hostname=$(hostname)

get-ident-all | while read -r line; do
    dev=$(echo "$line" | awk '{print $2}')
    uut=$(echo "$line" | awk '{print $3}')
    for user in "$@"; do

        target="${HOME}/${hostname}:${user}:${uut}"
        source="/mnt/afhba.${dev}/${uut}/000000/${dev}.00"

        echo "${target} --> ${source}"
        ln -s -f $source $target
    done
done