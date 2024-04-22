#!/bin/bash


COUNTER=0
DIR="WKPDA"



while true; do
    
    
    
    sudo mv ./${DIR}/${COUNTER}/${COUNTER}.txt  ./${DIR}/${COUNTER}.txt
     
    
    let COUNTER=COUNTER+1
    
    
    if [ $COUNTER -gt 10 ]; then
        break
    fi
done