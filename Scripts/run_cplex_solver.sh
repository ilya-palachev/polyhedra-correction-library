#!/bin/bash

PROBLEM_FILE=$1
SOLUTION_FILE_XML=$2
SOLUTION_FILE=$3

rm ${SOLUTION_FILE}
rm ${SOLUTION_FILE_XML}

echo "                        
read ${PROBLEM_FILE}
optimize
write ${SOLUTION_FILE_XML}
sol
y
q" | /opt/ibm/ILOG/CPLEX_Studio1261/cplex/bin/x86-64_linux/cplex 1>&2

cat ${SOLUTION_FILE_XML} |
	grep x_ |
	sed -e "s/[^_]*_//;s/_\([0-9]\)/ \1/;s/\".*value=\"/ /;s/\".*//" |
	sort -n |
	awk '{print $3}' > ${SOLUTION_FILE}

exit 0
