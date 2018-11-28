#!/bin/bash

S=`ipcs - s | egrep "0x[0-9a-f]+" | awk '{print $2}'`
M=`ipcs - m | egrep "0x[0-9a-f]+" | awk '{print $2}'`
Q=`ipcs - q | egrep "0x[0-9a-f]+" | awk '{print $2}'`

for id in $S; do
    printf "Removing: $id\n"
    ipcrm -s $id;
done

for id in $M; do
    ipcrm -m $id;
done

for id in $Q; do
    ipcrm -q $id;
done