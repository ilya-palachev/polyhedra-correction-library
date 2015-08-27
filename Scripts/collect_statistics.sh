#!/bin/bash

POLYHEDRON_NAMES="1_GIA Marq54 MSS_Cushion_0 MSS_MARQ_G_VS2_0.54_2012_09_18 Pear63"

TMP_RESULTS_FILE=$(mktemp)
for POLYHEDRON_NAME in ${POLYHEDRON_NAMES}
do
	TMP_FILE=$(mktemp)
	./Recoverer_test  -f ../poly-data-in/${POLYHEDRON_NAME}/shadow_contours.dat -b -x -s -r native -p l2 -c ../poly-data-in/${POLYHEDRON_NAME}/polyhedron.dat 2>&1 | grep '164\]' | sed -e "s/.* //g" > ${TMP_FILE}
	sed -i ${TMP_FILE} -e ':a;N;$!ba;s/\n/ /g'
	RESULT_LINE=$(cat ${TMP_FILE})
	echo "${POLYHEDRON_NAME} ${RESULT_LINE}" >> ${TMP_RESULTS_FILE}
done

cat ${TMP_RESULTS_FILE}
exit 0
