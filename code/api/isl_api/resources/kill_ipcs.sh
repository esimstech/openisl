#!/bin/bash

ME=`whoami`

IPCS_S=`ipcs -s | egrep "0x49[0-9a-f]+ [0-9]+" | grep $ME | cut -f2 -d" "`
IPCS_M=`ipcs -m | egrep "0x49[0-9a-f]+ [0-9]+" | grep $ME | cut -f2 -d" "`

for id in $IPCS_M; do
  echo "Removing shm $id"
  ipcrm -m $id
done

for id in $IPCS_S; do
  echo "Removing sem $id"
  ipcrm -s $id
done

echo "IPC listing"
ipcs
