#!/bin/bash


COUNTER=0
OUT_DIR="./RESULTS"

mkdir -p ${OUT_DIR}

cd ${OUT_DIR}

while true; do
    
    
    
    sudo taskset 0x4 ../spy >  ${COUNTER}.txt
     
    
    let COUNTER=COUNTER+1
    
    
    if [ $COUNTER -gt 10 ]; then
        break
    fi
done