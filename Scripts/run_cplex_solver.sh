#!/bin/bash

echo "                        
read $1
optimize
write $2
sol
q" | /opt/ibm/ILOG/CPLEX_Studio1261/cplex/bin/x86-64_linux/cplex 1>&2

cat $2 | grep x_ | sed -e "s/.*value=\"//g;s/\".*//g" > $3

exit 0
