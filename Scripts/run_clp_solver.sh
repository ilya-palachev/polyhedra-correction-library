#!/bin/bash

echo "                        
import $1
primalS
solution $2
q" | clp 1>&2

cat $2 | grep x_ | awk '{print $3}' > $3

exit 0
