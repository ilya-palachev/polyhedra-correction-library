#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/git/opensource/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/buildRelease

CORRECT_SIMPLIFIED_BODY=1 LD_LIBRARY_PATH=/home/ilya/git/opensource/Ipopt-3.12.1/install/lib/ ${WORKSPACE_DIR}/buildRelease/Recoverer_test -f poly-data-in/1_GIA/shadow_contours.dat -o poly-data-out/1_GIA -r native -p l2 -x -s -b -T 0.01 -z 0.01
